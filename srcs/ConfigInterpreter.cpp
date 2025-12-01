/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigInterpreter.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:28 by ego               #+#    #+#             */
/*   Updated: 2025/12/01 20:38:51 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigInterpreter.hpp"

/**
 * @brief Build an interpreter bound to a parsed block tree.
 *
 * @param blocks Blocks returned by ConfigParser::parse().
 */
ConfigInterpreter::ConfigInterpreter(const std::vector<Block> &blocks)
	:	_blocks(blocks)
{
	return ;
}

/**
 * @brief Convert parsed blocks into runtime ServerConfig objects.
 *
 * @throws ConfigError derivatives when any server block is invalid.
 * 
 * @return Vector of fully interpreted server configurations.
 */
std::vector<ServerConfig>	ConfigInterpreter::interpret()
{
	std::vector<ServerConfig>	servers;

	for (size_t i = 0;  i < _blocks.size(); ++i)
		servers.push_back(_parseServer(_blocks[i]));
	return (servers);
}

/**
 * @brief Interpret a single `server` block into a ServerConfig.
 *
 * Applies server-level directives and then parses nested location blocks.
 *
 * @throws ConfigError derivatives when server contents are invalid.
 *
 * @param block Parsed server block.
 *
 * @return Filled ServerConfig object.
 */
ServerConfig	ConfigInterpreter::_parseServer(const Block &block)
{
	ServerConfig			conf;
	std::set<std::string>	already_applied;
	bool					seen_error_pages_block = false;

	for (size_t i = 0; i < block.directives.size(); ++i)
		_applyServerDirective(conf, block.directives[i], already_applied);
	for (size_t i = 0;  i < block.children.size(); ++i)
	{
		const Block	&child  = block.children[i];

		if (child.type == "error_pages")
			_parseErrorPages(conf, child, seen_error_pages_block);
		else if (child.type == "location")
		{
			Location	loc = _parseLocation(child, conf);
			if (utils::mapHasEntry(conf.locations, loc.path))
				throw DuplicateError(block.line, "location '" + loc.path + "'");
			conf.locations[loc.path] = loc;
		}
		else
			throw UnknownDirectiveError(child.line, child.type);
	}
	return (conf);
}

/**
 * @brief Parse and apply an `error_pages` block.
 *
 * @throws DuplicateError when multiple error_pages blocks are defined.
 * @throws MissingArgumentError or TooManyArgumentsError on arity issues.
 */
void	ConfigInterpreter::_parseErrorPages(ServerConfig &conf, const Block &block, bool &seen_error_pages_block)
{
	if (seen_error_pages_block)
		throw DuplicateError(block.line, "error_pages");
	seen_error_pages_block = true;
	for (size_t j = 0; j < block.directives.size(); ++j)
	{
		if (block.directives[j].args.empty())
			throw MissingArgumentError(block.directives[j].line, "error_pages");
		if (block.directives[j].args.size() > 1)
			throw TooManyArgumentsError(block.directives[j].line, "error_pages");
		_parseErrorPage(conf, block.directives[j]);
	}
}

/**
 * @brief Interpret a `location` block using server defaults.
 *
 * Starts from inherited defaults, then applies location-specific directives.
 *
 * @throws ConfigError derivatives when location contents are invalid.
 *
 * @param block Parsed location block.
 * @param server Server defaults to inherit from.
 *
 * @return Filled Location structure.
 */
Location	ConfigInterpreter::_parseLocation(const Block &block, const ServerConfig &server)
{
	Location				loc;
	std::set<std::string>	already_applied;

	loc.path = block.path;
	loc.root = server.root;
	loc.index = server.index;
	loc.autoindex = server.autoindex;
	loc.methods = GET;
	loc.has_redirect = false;
	for (size_t i = 0;  i < block.directives.size(); ++i)
		_applyLocationDirective(loc, block.directives[i], already_applied);
	return (loc);
}

/**
 * @brief Apply one server-level directive to the given config.
 *
 * Validates arity, detects duplicates, and dispatches to directive-specific
 * handlers. Server-level error pages are provided via the `error_pages` block.
 *
 * @throws ConfigError derivatives when the directive is malformed or duplicated.
 *
 * @param conf Server configuration being built.
 * @param d Directive to apply.
 */
void	ConfigInterpreter::_applyServerDirective(ServerConfig &conf, const Directive &d,
				std::set<std::string> &already_applied)
{
	if (d.args.size() < 1)
		throw MissingArgumentError(d.line, d.name);
	std::string	name = utils::toLower(d.name);
	if (name ==  "listen")							_parseListen(conf, d);
	else if (name == "server_name")					conf.server_name = d.args[0];
	else if (name == "root")						conf.root = d.args[0];
	else if (name == "index")						conf.index = d.args[0];
	else if (name == "autoindex")					conf.autoindex = (d.args[0] == "on");
	else if (name == "client_limit")				conf.max_clients = std::atoi(d.args[0].c_str());
	else if (name == "client_max_body_size")		conf.client_max_body_size = _parseSizeWithSuffix(d.args[0], d.line);
	else if (name == "timeout")						conf.timeout = static_cast<long>(_parseSizeWithSuffix(d.args[0], d.line));
	else if (name == "client_header_buffer_size")	conf.client_header_buffer_size = _parseSizeWithSuffix(d.args[0], d.line);
	else if (name == "client_body_buffer_size")		conf.client_body_buffer_size = _parseSizeWithSuffix(d.args[0], d.line);
	else
		throw UnknownDirectiveError(d.line, d.name);
	if (already_applied.count(name))
		throw DuplicateError(d.line, name);
	already_applied.insert(name);
	if (d.args.size() > 1)
		throw TooManyArgumentsError(d.line, d.name);
	return ;
}

/**
 * @brief Apply one location-level directive.
 *
 * Validates arity and updates the provided Location accordingly.
 *
 * @throws ConfigError derivatives when the directive is malformed or duplicated.
 *
 * @param loc Location being configured.
 * @param d Directive to apply.
 */
void	ConfigInterpreter::_applyLocationDirective(Location &loc, const Directive &d,
				std::set<std::string> &already_applied)
{
	if (d.args.size() < 1)
		throw MissingArgumentError(d.line, d.name);
	std::string	name = utils::toLower(d.name);
	if (name == "root")					loc.root = d.args[0];
	else if (name == "index")			loc.index  = d.args[0];
	else if (name == "autoindex")		loc.autoindex = (d.args[0] == "on");
	else if (name == "limit_except")	_parseMethod(loc, d);
	else if (name == "upload_path")		loc.upload_path = d.args[0];
	else if (name == "cgi_pass")		loc.cgi_pass = d.args[0];
	else if (name == "return")			_parseReturn(loc, d);
	else
		throw UnknownDirectiveError(d.line, d.name);
	if (already_applied.count(name))
		throw DuplicateError(d.line, name);
	already_applied.insert(name);
	return ;
}

/**
 * @brief Parse a listen directive of the form `host:port`.
 *
 * @throws InvalidListenFormatError on missing colon or empty parts.
 * @throws InvalidListenHostError on bad IPv4 address.
 * @throws InvalidListenPortError on out-of-range or non-numeric port.
 *
 * @param conf Server configuration to update.
 * @param d Directive carrying the listen value.
 */
void	ConfigInterpreter::_parseListen(ServerConfig &conf, const Directive &d)
{
	const std::string	&v = d.args[0];
	size_t				pos = v.find(':');

	if (pos == std::string::npos)
		throw InvalidListenFormatError(d.line, v);
	std::string	host = v.substr(0, pos);
	std::string	port_str = v.substr(pos + 1);

	if (host.empty() || port_str.empty())
		throw InvalidListenFormatError(d.line, v);
	std::vector<std::string> parts = utils::stringSplit(host, ".");
	if (parts.size() != 4)
		throw InvalidListenHostError(d.line, host);
	for (size_t i = 0; i < parts.size(); ++i)
	{
		char	*endptr = 0;
		long	octet = std::strtol(parts[i].c_str(), &endptr, 10);

		if (!endptr || *endptr != '\0' || octet < 0 || octet > 255 || parts[i].length() > 3)
			throw InvalidListenHostError(d.line, host);
	}
	char	*endptr = 0;
	long	port = std::strtol(port_str.c_str(), &endptr, 10);
	if (!endptr || *endptr != '\0' || port < 1 || port > 65535)
		throw InvalidListenPortError(d.line, port_str);
	conf.listen_host_string = host;
	conf.listen_host = static_cast<uint32_t>(htonl(
		(static_cast<uint32_t>(std::atoi(parts[0].c_str())) << 24) |
		(static_cast<uint32_t>(std::atoi(parts[1].c_str())) << 16) |
		(static_cast<uint32_t>(std::atoi(parts[2].c_str())) << 8)  |
		static_cast<uint32_t>(std::atoi(parts[3].c_str()))
	));
	conf.listen_port = static_cast<int>(port);
	return ;
}

/**
 * @brief Parse `error_page` directive and register the mapping.
 *
 * @throws MissingArgumentError or TooManyArgumentsError on arity issues.
 * @throws DuplicateError when the status code is redefined.
 * @throws InvalidStatusCodeError or UnknownStatusCodeError when the code is invalid.
 *
 * @param conf Server configuration to update.
 * @param d Directive containing status code and path.
 */
void	ConfigInterpreter::_parseErrorPage(ServerConfig &conf, const Directive &d)
{
	const std::string	&code_str = d.name;
	char				*endptr = 0;
	long				code_long = std::strtol(code_str.c_str(), &endptr, 10);

	if (!endptr || *endptr != '\0')
		throw InvalidStatusCodeError(d.line, code_str);
	if (code_long < 100 || code_long > 999)
		throw InvalidStatusCodeError(d.line, code_str);
	HttpStatus	status = utils::strToHttpStatus(code_str);
	if (status == HTTP_UNKNOWN_STATUS)
		throw UnknownStatusCodeError(d.line, code_str);
	const std::string	&path  =  d.args[0];
	int 				code = static_cast<int>(code_long);
	if (conf.error_pages.find(code) != conf.error_pages.end())
		throw DuplicateError(d.line, utils::toString(code));
	conf.error_pages[code] = path;
	return ;
}

/**
 * @brief Parse `limit_except` directive and build a method mask.
 *
 * @throws UnknownMethodError on unsupported methods.
 *
 * @param loc Location to update.
 * @param d Directive holding the allowed methods.
 */
void	ConfigInterpreter::_parseMethod(Location &loc, const Directive &d)
{
	Method	mask = UNKNOWN;
	for (size_t i = 0; i < d.args.size(); ++i)
	{
		if (d.args[i] == "GET")			mask |= GET;
		else if (d.args[i] == "POST")	mask |= POST;
		else if (d.args[i] == "DELETE")	mask |= DELETE;
		else							throw UnknownMethodError(d.line, d.args[i]);
	}
	loc.methods = mask;
}

/**
 * @brief Parse `return` directive and store redirect info.
 *
 * @throws MissingArgumentError or TooManyArgumentsError on arity issues.
 *
 * @param loc Location to update.
 * @param d Directive containing status code and URL.
 */
void	ConfigInterpreter::_parseReturn(Location &loc, const Directive &d)
{
	if (d.args.size() < 2)
		throw MissingArgumentError(d.line, d.name);
	if (d.args.size() >  2)
		throw TooManyArgumentsError(d.line, d.name);
	loc.redirect.code = std::atoi(d.args[0].c_str());
	loc.redirect.url = d.args[1];
	loc.has_redirect = true;
	return ;
}

/**
 * @brief Parse size strings optionally suffixed by K, M, or s.
 *
 * Suffix `s` is treated as seconds and returns the numeric value in
 * milliseconds (x1000), suitable for timeout fields.
 *
 * @throws InvalidSizeError or InvalidSizeSuffixError on malformed size.
 * @throws UnknownSizeError on unsupported suffix.
 *
 * @param s String to parse (e.g., "1024", "8K", "1M", "30s").
 * @return Size in bytes.
 */
size_t	ConfigInterpreter::_parseSizeWithSuffix(const std::string &s, int line)  const
{
	char	*endptr;
	size_t	multiplier;
	size_t	base = std::strtoul(s.c_str(), &endptr, 10);

	if (base == 0)
		throw InvalidSizeError(line, s);
	if (!*endptr)
		return (base);
	if (*(endptr + 1))
		throw InvalidSizeSuffixError(line, s);
	if (tolower(*endptr) == 'k')
		multiplier = 1024;
	else if (tolower(*endptr) == 'm')
		multiplier = 1024 * 1024;
	else if (tolower(*endptr) == 's')
		multiplier = 1000;
	else
		throw UnknownSizeError(line, s);
	return (base * multiplier);
}
