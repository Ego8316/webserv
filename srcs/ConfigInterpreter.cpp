/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigInterpreter.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:28 by ego               #+#    #+#             */
/*   Updated: 2025/11/27 04:29:02 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigInterpreter.hpp"

ConfigInterpreter::ConfigInterpreter(const std::vector<Block> &blocks)
	:	_blocks(blocks)
{
	return ;
}

std::vector<ServerConfig>	ConfigInterpreter::interpret()
{
	std::vector<ServerConfig>	servers;

	for (size_t i = 0;  i < _blocks.size(); ++i)
		servers.push_back(_parseServer(_blocks[i]));
	return (servers);
}

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
	else if (name == "autoindex")					conf.index = d.args[0];
	else if (name == "client_max_body_size")		conf.client_max_body_size = _parseSizeWithSuffix(d.args[0]);
	else if (name == "client_header_timeout")		conf.client_header_timeout = std::atol(d.args[0].c_str());
	else if (name == "client_body_timeout")			conf.client_body_timeout = std::atol(d.args[0].c_str());
	else if (name == "send_timeout")				conf.send_timeout = std::atol(d.args[0].c_str());
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
	else
		throw std::runtime_error("Unknown size suffix at line ...");
	return (base * multiplier);
}
