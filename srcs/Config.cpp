/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/24 00:32:01 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

int	Config::_line_number = 0;

Config::FieldHandler Config::_fields[] = {
	{"ip", FieldHandler::INT, true, false, 0, 0,
		{ .uintField = &Config::ip }, { .ulongValue = INADDR_ANY }},
	{"port", FieldHandler::INT, true, false, MIN_PORT, MAX_PORT,
		{ .intField = &Config::port_number }, { .intValue = DEFAULT_PORT }},
	{"domain", FieldHandler::INT, true, false, 0, 0,
		{ .intField = &Config::domain }, { .intValue = AF_INET }},
	{"type", FieldHandler::INT, true, false, 0, 0,
		{ .intField = &Config::type }, { .intValue = SOCK_STREAM }},
	{"protocol", FieldHandler::INT, true, false, 0, 0,
		{ .intField = &Config::protocol }, { .intValue = 0 }},
	{"max_header_size", FieldHandler::INT, false, false, MIN_MAX_HEADER_SIZE, MAX_MAX_HEADER_SIZE,
		{ .ulongField = &Config::max_header_size }, { .ulongValue = DEFAULT_MAX_HEADER_SIZE }},
	{"max_body_size", FieldHandler::INT, false, false, MIN_MAX_BODY_SIZE, MAX_MAX_BODY_SIZE,
		{ .ulongField = &Config::max_body_size }, { .ulongValue = DEFAULT_MAX_BODY_SIZE }},
	{"client_limit", FieldHandler::INT, false, false, MIN_CLIENT_LIMIT, MAX_CLIENT_LIMIT,
		{ .intField = &Config::client_limit }, { .intValue = DEFAULT_CLIENT_LIMIT }},
	{"processing_time_limit", FieldHandler::LONG, false, false, MIN_PROCESSING_TIME_LIMIT, MAX_PROCESSING_TIME_LIMIT,
		{ .longField = &Config::processing_time_limit }, { .longValue = DEFAULT_PROCESSING_TIME_LIMIT }},
	{"max_request_time", FieldHandler::LONG, false, false, MIN_MAX_REQUEST_TIME, MAX_MAX_REQUEST_TIME,
		{ .longField = &Config::max_request_time }, { .longValue = DEFAULT_MAX_REQUEST_TIME }},
	{"incoming_queue_backlog", FieldHandler::INT, false, false, MIN_INCOMING_QUEUE_BACKLOG, MAX_INCOMING_QUEUE_BACKLOG,
		{ .intField = &Config::incoming_queue_backlog }, { .intValue = DEFAULT_INCOMING_QUEUE_BACKLOG }},
	{"buffer_size", FieldHandler::INT, false, false, MIN_BUFFER_SIZE, MAX_BUFFER_SIZE,
		{ .intField = &Config::buffer_size }, { .intValue = DEFAULT_BUFFER_SIZE }},
	{"cookie_sessions_max", FieldHandler::INT, false, false, MIN_COOKIE_SESSIONS_MAX, MAX_COOKIE_SESSIONS_MAX,
		{ .intField = &Config::cookie_sessions_max }, { .intValue = DEFAULT_COOKIE_SESSIONS_MAX }},
	{"cookie_life_time", FieldHandler::INT, false, false, MIN_COOKIE_LIFETIME, MAX_COOKIE_LIFETIME,
		{ .intField = &Config::cookie_life_time }, { .intValue = DEFAULT_MAX_COOKIE_LIFETIME }},
	{"server_home", FieldHandler::STRING, true, false, 0, 0,
		{ .stringField = &Config::server_home },{ .strValue = DEFAULT_SERVER_HOME }},
	{"enable_listdir", FieldHandler::BOOL, false, false, 0, 0,
		{ .boolField = &Config::enable_listdir }, { .boolValue = DEFAULT_ENABLE_LISTDIR }},
	{"default_page", FieldHandler::STRING, false, false, 0, 0,
		{ .stringField = &Config::default_page }, { .strValue = DEFAULT_DEFAULT_PAGE }}
};

		std::map<int, std::string>				default_error_pages;
		std::vector<Method>						accepted_methods;
		std::map<std::string, Redirection>		http_redir;

Config::Config(const std::string &conf)
{
	std::istringstream	conf_stream(conf);
	std::string			line;
	std::string			field, equal, value;

	while (std::getline(conf_stream, line))
	{
		++_line_number;
		line = utils::stringTrim(line, " \t\n");
		if (line.empty()) continue ;
		std::istringstream	line_stream(line);
		if (!(line_stream >> field >> equal >> value) || (equal != "=" && equal != ":"))
			throw Error("Invalid line format");
		bool	matched = false;
		for (size_t i = 0; i < sizeof(_fields) / sizeof(_fields[0]); ++i)
		{
			FieldHandler	&fh = this->_fields[i];
			if (fh.name == utils::toLower(field))
			{
				if (fh.found == true)
					throw Error("Duplicate");
				fh.found = true;
				this->_assignValue(fh, value);
				matched = true;
				break ;
			}
		}
		if (!matched)
			throw Error("Unknown config field: " + field);
	}
	for (size_t i = 0; i < sizeof(_fields) / sizeof(_fields[0]); ++i)
	{
		FieldHandler	&fh = _fields[i];
		if (!fh.found && fh.required)
			throw Error("Missing config field: " + fh.name);
		if (!fh.found)
			this->assignDefault(fh);
	}
}

// Config::Config(std::string config)
// {
// 	std::istringstream	conf_stream(config);
// 	std::string			newline;
// 	std::string			field, equal, value;

// 	while (std::getline(conf_stream, newline))
// 	{
// 		if (utils::stringTrim(newline, " \t\n").length() == 0)
// 			continue ;
// 		std::istringstream 			line(newline);
	
// 		if (!(line >> field >> equal >> value) || (equal != "=" && equal != ":"))
// 		{
// 			std::cerr << "Could not parse config line " << newline << " in global"   << std::endl;
// 			continue;
// 		}
// 		if (field == "IP")
// 			this->setIP(value);
// 		if (field == "PORT")
// 			this->port_number = atoi(value.c_str());
// 		else if (field == "DOMAIN")
// 		{
// 			if (value == "AF_INET")
// 				this->domain = AF_INET;
// 		}
// 		else if (field == "TYPE")
// 		{
// 			if (value == "SOCK_STREAM")
// 				this->type = SOCK_STREAM;
// 		}
// 		else if (field == "PROTOCOL")
// 			this->protocol = atoi(value.c_str());
// 		else if (field == "METHODS")
// 			this->parseMethod(conf_stream);
// 		else if (field == "MAX_HEADER_SIZE")
// 			this->max_header_size = atoi(value.c_str());
// 		else if (field == "MAX_BODY_SIZE")
// 			this->max_body_size = atoi(value.c_str());
// 		else if (field == "CLIENT_LIMIT")
// 			this->client_limit = atoi(value.c_str());
// 		else if (field == "PROCESSING_TIME_LIMIT")
// 			this->processing_time_limit = atoi(value.c_str());
// 		else if (field == "MAX_REQUEST_TIME")
// 			this->max_request_time = atoi(value.c_str());
// 		else if (field == "INCOMMING_QUEUE_BACKLOG")
// 			this->incoming_queue_backlog = atoi(value.c_str());
// 		else if (field == "COOKIE_SESSIONS_MAX")
// 			this->cookie_sessions_max = atoi(value.c_str());
// 		else if (field == "COOKIE_LIFE_TIME")
// 			this->cookie_life_time = atoi(value.c_str());
// 		else if (field == "BUFFER_SIZE")
// 			this->buffer_size = atoi(value.c_str());
// 		else if (field == "SERVER_HOME")
// 			this->server_home = value;
// 		else if (field == "ENABLE_LISTDIR")
// 			this->enable_listdir = ((value == "1") || (value == "true") || (value == "TRUE") || (value == "True"));
// 		else if (field == "DEFAULT_PAGE")
// 			this->default_page = value;
// 		else if (field == "DEFAULT_ERROR_PAGES" && value == "list")
// 			this->parseDefaultErrorPages(conf_stream);
// 		else if (field == "HTTP_REDIR")
// 			this->parseHttpRedir(conf_stream);
// 	}
// }

Config::Config(const Config &other)
{
	*this = other;
}

Config	&Config::operator=(const Config &other)
{
	if (this != &other)
	{
		this->ip = other.ip;
		this->port_number = other.port_number;
		this->domain = other.domain;
		this->type = other.type;
		this->protocol = other.protocol;
		this->max_header_size = other.max_header_size;
		this->max_body_size = other.max_body_size;
		this->client_limit = other.client_limit;
		this->processing_time_limit = other.processing_time_limit;
		this->incoming_queue_backlog = other.incoming_queue_backlog;
		this->buffer_size = other.buffer_size;
		this->cookie_sessions_max = other.cookie_sessions_max;
		this->cookie_life_time = other.cookie_life_time;
		this->server_home = other.server_home;
		this->enable_listdir = other.enable_listdir;
		this->default_page = other.default_page;
		this->default_error_pages = other.default_error_pages;
		this->http_redir = other.http_redir;
		this->accepted_methods = other.accepted_methods;
	}
	return (*this);
}

Config::~Config() {}

const std::map<std::string, Redirection>	&Config::getRedirections() const
{
	return (this->http_redir);
}

void	Config::_assignValue(FieldHandler &fh, const std::string &value)
{
	return ;
}

void	Config::_setIP(std::string ip_str)
{
	int					octets[4];
	char				dot;
	std::istringstream	iss(ip_str);
	unsigned int		ip_uint = 0;
	
	if (ip_str == "INADDR_ANY")
	{
		this->ip = INADDR_ANY;
		return ;
	}
	else if (ip_str == "INADDR_NONE")
	{
		this->ip = INADDR_NONE;
		return ;
	}
	if ((iss >> octets[0] >> dot >> octets[1] >> dot >> octets[2] >> dot >> octets[3])
		&& dot == '.' && iss.eof())
	{
		for (int i = 0; i < 4; ++i)
		{
			if (octets[i] < 0 || octets[i] > 255)
				throw Error("Invalid IP address: " + ip_str);
			ip_uint += octets[i] * pow(2, (3 - i) * 8);
		}
	}
	this->ip = htonl(ip_uint);
}

void	Config::_parseDefaultErrorPages(std::istringstream &conf_stream)
{
	std::string	newline;
	int			field;
	std::string	equal, value;

	while (std::getline(conf_stream, newline))
	{
		++_line_number;
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t\n") == "end")
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> field >> equal >> value) || (equal != "=" && equal != ":"))
			throw Error("Invalid line format");
		// if (utils::httpStatusToStr(field) == "Unknown")
		if (field < 200 || field > 600)
			throw Error("Unknown or unsupported error number in default error pages definition");
		// TODO mieux checker
		this->default_error_pages[field] = value;
	}
}

void	Config::_parseMethod(std::istringstream &conf_stream)
{
	std::string	newline;
	std::string	field;

	while (std::getline(conf_stream, newline))
	{
		++_line_number;
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t\n") == "end")
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> field))
			throw Error("Invalid line format");
		if (utils::strToMethod(field) == UNKNOWN)
			throw Error("Unknown method: " + field);
		this->accepted_methods.push_back(utils::strToMethod(field));
	}
}

void		Config::_parseHttpRedir(std::istringstream &conf_stream)
{
	std::string	newline;
	std::string	path, equal, dest;
	int			error_code;

	while (std::getline(conf_stream, newline))
	{
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t\n") == "end")
			return ;
		std::istringstream	line(newline);
		
		if (!(line >> path >> equal >> dest >> error_code) || (equal != "=" && equal != ":"))
			throw Error("Invalid line format");
		this->http_redir[path].dest = dest;
		if ((300 <= error_code && error_code <= 302) || error_code == 308)
			this->http_redir[path].error_code = static_cast<HttpStatus>(error_code);
		else
			this->http_redir[path].error_code = HTTP_REDIRECT;
		if (newline.find("}") != std::string::npos)
			return ;
	}
}

bool	Config::isAcceptedMethod(Method element) const
{
	for (unsigned int i = 0; i < this->accepted_methods.size(); ++i)
		if (element == this->accepted_methods[i])
			return (true);
	return (false);
}

std::ostream	&operator<<(std::ostream &os, const Config &item)
{
	os << "IP :" << item.ip << std::endl;
	os << "port_number :" << item.port_number << std::endl;
	os << "domain :" << item.domain << std::endl;
	os << "type :" << item.type << std::endl;
	os << "protocol :" << item.protocol << std::endl;
	os << "client_limit :" << item.client_limit << std::endl;
	os << "incoming_queue_backlog :" << item.incoming_queue_backlog << std::endl;
	os << "buffer_size :" << item.buffer_size << std::endl;
	os << "cookie_sessions_max :" << item.cookie_sessions_max << std::endl;
	os << "cookie_life_time :" << item.cookie_life_time << std::endl;
	os << "ServHome :" << item.server_home << std::endl;
	os << "enable_listdir :" << item.enable_listdir << std::endl;
	os << "default_page :" << item.default_page << std::endl;
	os << "Default error pages:" << std::endl;
	std::map<int, std::string>		dep = item.default_error_pages;
	std::map<int, std::string>::iterator		dep_it;
	for (dep_it = dep.begin(); dep_it != dep.end(); ++dep_it)
		os << dep_it->first << ": " << dep_it->second << "\n";
	os << std::endl;
	os << "Accepted Methods:" << std::endl;
	for (unsigned int i = 0; i < item.accepted_methods.size(); ++i)
		os << item.accepted_methods[i] << "\n";
	os << std::endl;
	os << "Redirections:" << std::endl;
	std::map<std::string, Redirection>		red = item.http_redir;
	std::map<std::string, Redirection>::iterator	red_it;
	for (red_it = red.begin(); red_it != red.end(); ++red_it)
		os << red_it->first << " -> " << red_it->second.dest << " with errcode " << red_it->second.error_code << "\n";
	os << std::endl;
	return (os);
}
