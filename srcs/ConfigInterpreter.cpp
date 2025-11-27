/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigInterpreter.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:28 by ego               #+#    #+#             */
/*   Updated: 2025/11/27 04:43:40 by ego              ###   ########.fr       */
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
 * @param block Parsed server block.
 *
 * @return Filled ServerConfig object.
 */
ServerConfig	ConfigInterpreter::_parseServer(const Block &block)
{
	ServerConfig	conf;

	for (size_t i = 0; i < block.directives.size(); ++i)
		_applyServerDirective(conf, block.directives[i]);
	for (size_t i = 0;  i < block.children.size(); ++i)
	{
		const Block	&location_block  = block.children[i];
		Location	loc = _parseLocation(location_block, conf);
		if (utils::mapHasEntry(conf.locations, loc.path))
			throw std::runtime_error("Redefinition of location '"
				+ loc.path + "' at line " + utils::toString(block.line));
		conf.locations[loc.path] = loc;
	}
	return (conf);
}

/**
 * @brief Interpret a `location` block using server defaults.
 *
 * Starts from inherited defaults, then applies location-specific directives.
 *
 * @param block Parsed location block.
 * @param server Server defaults to inherit from.
 *
 * @return Filled Location structure.
 */
Location	ConfigInterpreter::_parseLocation(const Block &block, const ServerConfig &server)
{
	Location	loc;

	loc.path = block.path;
	loc.root = server.root;
	loc.index = server.index;
	loc.autoindex = server.autoindex;
	loc.methods = GET;
	loc.has_redirect = false;
	for (size_t i = 0;  i < block.directives.size(); ++i)
		_applyLocationDirective(loc, block.directives[i]);
	return (loc);
}

/**
 * @brief Apply one server-level directive to the given config.
 *
 * Validates arity, detects duplicates (except error_page), and dispatches to
 * directive-specific handlers.
 *
 * @param conf Server configuration being built.
 * @param d Directive to apply.
 */
void	ConfigInterpreter::_applyServerDirective(ServerConfig &conf, const Directive &d)
{
	static std::vector<std::string>	already_applied;

	if (d.args.size() < 1)
		throw std::runtime_error("Missing argument for directive '" + d.name
			+ "' at line " + utils::toString(d.line));
	std::string	name = utils::toLower(d.name);
	if (name ==  "listen")							_parseListen(conf, d);
	else if (name == "server_name")					conf.server_name = d.args[0];
	else if (name == "root")						conf.root = d.args[0];
	else if (name == "index")						conf.index = d.args[0];
	else if (name == "autoindex")					conf.autoindex = (d.args[0] == "on");
	else if (name == "client_max_body_size")		conf.client_max_body_size = _parseSizeWithSuffix(d.args[0]);
	else if (name == "client_header_timeout")		conf.client_header_timeout = static_cast<long>(_parseSizeWithSuffix(d.args[0]));
	else if (name == "client_body_timeout")			conf.client_body_timeout = static_cast<long>(_parseSizeWithSuffix(d.args[0]));
	else if (name == "send_timeout")				conf.send_timeout = static_cast<long>(_parseSizeWithSuffix(d.args[0]));
	else if (name == "client_header_buffer_size")	conf.client_header_buffer_size = _parseSizeWithSuffix(d.args[0]);
	else if (name == "client_body_buffer_size")		conf.client_body_buffer_size = _parseSizeWithSuffix(d.args[0]);
	else if (name == "error_page")					_parseErrorPage(conf, d);
	else
		throw std::runtime_error("Unknown directive '" + d.name
			+ "' at line " + utils::toString(d.line));
	if (std::find(already_applied.begin(), already_applied.end(), name) != already_applied.end()
		&& name != "error_page")
		throw std::runtime_error("Duplicate directive '" + name
			+ "' at line " + utils::toString(d.line));
	already_applied.push_back(name);
	if (d.args.size() > 1)
		throw std::runtime_error("Too many arguments for directive '" + d.name
			+ "' at line " + utils::toString(d.line));
	return ;
}

/**
 * @brief Apply one location-level directive.
 *
 * Validates arity and updates the provided Location accordingly.
 *
 * @param loc Location being configured.
 * @param d Directive to apply.
 */
void	ConfigInterpreter::_applyLocationDirective(Location &loc, const Directive &d)
{
	if (d.args.size() < 1)
		throw std::runtime_error("Missing argument for directive '" + d.name
			+ "' at line " + utils::toString(d.line));
	std::string	name = utils::toLower(d.name);
	if (name == "root")					loc.root = d.args[0];
	else if (name == "index")			loc.index  = d.args[0];
	else if (name == "autoindex")		loc.autoindex = (d.args[0] == "on");
	else if (name == "limit_except")	_parseMethod(loc, d);
	else if (name == "upload_path")		loc.upload_path = d.args[0];
	else if (name == "cgi_pass")		loc.cgi_pass = d.args[0];
	else if (name == "return")			_parseReturn(loc, d);
	else
		throw std::runtime_error("Unknown directive '" + d.name
			+ "' at line " + utils::toString(d.line));
	return ;
}

/**
 * @brief Parse a listen directive of the form `host:port`.
 *
 * @param conf Server configuration to update.
 * @param d Directive carrying the listen value.
 */
void	ConfigInterpreter::_parseListen(ServerConfig &conf, const Directive &d)
{
	const std::string	&v = d.args[0];
	size_t				pos = v.find(':');

	if (pos == std::string::npos)
		throw  std::runtime_error("NUL");
	conf.listen_host = v.substr(0, pos);
	conf.listen_port = std::atoi(v.substr(pos + 1).c_str());
	return ;
}

/**
 * @brief Parse `error_page` directive and register the mapping.
 *
 * @param conf Server configuration to update.
 * @param d Directive containing status code and path.
 */
void	ConfigInterpreter::_parseErrorPage(ServerConfig &conf, const Directive &d)
{
	if (d.args.size() < 2)
		throw std::runtime_error("Missing argument for directive '" + d.name
			+ "' at line " + utils::toString(d.line));
	if (d.args.size() >  2)
		throw std::runtime_error("Too many arguments for directive '" + d.name
			+ "' at line " + utils::toString(d.line));
	int 				code = std::atoi(d.args[0].c_str());
	const std::string	&path  =  d.args[1];
	conf.error_pages[code] = path;
	return ;
}

/**
 * @brief Parse `limit_except` directive and build a method mask.
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
		else
			throw std::runtime_error("Unknown method '" + d.args[i]
				+ "' at line " + utils::toString(d.line));
	}
	loc.methods = mask;
}

/**
 * @brief Parse `return` directive and store redirect info.
 *
 * @param loc Location to update.
 * @param d Directive containing status code and URL.
 */
void	ConfigInterpreter::_parseReturn(Location &loc, const Directive &d)
{
	if (d.args.size() < 2)
		throw std::runtime_error("Missing argument for directive '" + d.name
			+ "' at line " + utils::toString(d.line));
	if (d.args.size() >  2)
		throw std::runtime_error("Too many arguments for directive '" + d.name
			+ "' at line " + utils::toString(d.line));
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
 * @param s String to parse (e.g., "1024", "8K", "1M", "30s").
 *
 * @return Size in bytes.
 */
size_t	ConfigInterpreter::_parseSizeWithSuffix(const std::string &s)  const
{
	char	*endptr;
	size_t	multiplier;
	size_t	base = std::strtoul(s.c_str(), &endptr, 10);

	if (base == 0)
		throw std::runtime_error("Invalid size at line ...");
	if (!*endptr)
		return (base);
	if (*(endptr + 1))
		throw std::runtime_error("Invalid size suffix at line ...");
	if (tolower(*endptr) == 'k')
		multiplier = 1024;
	else if (tolower(*endptr) == 'm')
		multiplier = 1024 * 1024;
	else if (tolower(*endptr) == 's')
		multiplier = 1000;
	else
		throw std::runtime_error("Unknown size suffix at line ...");
	return (base * multiplier);
}
