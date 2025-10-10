/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/10 14:45:54 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(std::string config)
{
	std::istringstream	conf_stream(config);
	std::string			newline;
	std::string			field, equal, value;

	this->parse_error = NONE;
	while (std::getline(conf_stream, newline))
	{
		if (utils::stringTrim(newline, " \t").length() == 0)
			continue ;
		std::istringstream 			line(newline);
	
		if (!(line >> field >> equal >> value) || (equal != "=" && equal != ":"))
		{
			std::cerr << "Could not parse config line " << newline << std::endl;
			continue;
		}
		if (field == "IP")
			this->setIP(value);
		if (field == "PORT")
			this->port_number = atoi(value.c_str());
		if (field == "HOST_NAME")
			this->host_name = value;
		else if (field == "DOMAIN")
		{
			if (value == "AF_INET")
				this->domain = AF_INET;
		}
		else if (field == "TYPE")
		{
			if (value == "SOCK_STREAM")
				this->type = SOCK_STREAM;
		}
		else if (field == "PROTOCOL")
			this->protocol = atoi(value.c_str());
		else if (field == "CLIENT_LIMIT")
			this->client_limit = atoi(value.c_str());
		else if (field == "INCOMMING_QUEUE_BACKLOG")
			this->incoming_queue_backlog = atoi(value.c_str());
		else if (field == "COOKIE_SESSIONS_MAX")
			this->cookie_sessions_max = atoi(value.c_str());
		else if (field == "COOKIE_LIFE_TIME")
			this->cookie_life_time = atoi(value.c_str());
		else if (field == "BUFFER_SIZE")
			this->buffer_size = atoi(value.c_str());
		else if (field == "SERVER_HOME")
			this->server_home = value;
		else if (field == "ENABLE_LISTDIR")
			this->enable_listdir = ((value == "1") || (value == "true") || (value == "TRUE") || (value == "True"));
		else if (field == "DEFAULT_PAGE")
			this->default_page = value;
		else if (field == "DEFAULT_ERROR_PAGES" && value == "list")
			this->parseDefaultErrorPages(conf_stream);
		else if (field == "ACCEPT")
			this->parseAccept(conf_stream);
		else if (field == "HTTP_REDIR")
			this->parseHttpRedir(conf_stream);
	}
}

Config::Config(const Config &other)
{
	this->port_number = other.port_number;
	this->domain = other.domain;
	this->host_name = other.host_name;
	this->type = other.type;
	this->protocol = other.protocol;
	this->client_limit = other.client_limit;
	this->incoming_queue_backlog = other.incoming_queue_backlog;
	this->buffer_size = other.buffer_size;
	this->cookie_sessions_max = other.cookie_sessions_max;
	this->cookie_life_time = other.cookie_life_time;
	this->server_home = other.server_home;
	this->enable_listdir = other.enable_listdir;
	this->default_page = other.default_page;
	this->default_error_pages = other.default_error_pages;
	this->accept_list = other.accept_list;
	this->http_redir = other.http_redir;
	this->parse_error = other.parse_error;
}

Config &Config::operator=(const Config &other)
{
	if (this != &other)
	{
		this->port_number = other.port_number;
		this->domain = other.domain;
		this->host_name = other.host_name;
		this->type = other.type;
		this->protocol = other.protocol;
		this->client_limit = other.client_limit;
		this->incoming_queue_backlog = other.incoming_queue_backlog;
		this->buffer_size = other.buffer_size;
		this->cookie_sessions_max = other.cookie_sessions_max;
		this->cookie_life_time = other.cookie_life_time;
		this->server_home = other.server_home;
		this->enable_listdir = other.enable_listdir;
		this->default_page = other.default_page;
		this->default_error_pages = other.default_error_pages;
		this->http_redir = other.http_redir;
		this->accept_list = other.accept_list;
		this->parse_error = other.parse_error;
	}
	return (*this);
}

Config::~Config() {}


ParseError		Config::getParseError()
{
	return (this->parse_error);
}

void			Config::setIP(std::string ip_str)
{
	int octets[4];
    char dot;
    std::istringstream iss(ip_str);
	unsigned int	ip_uint = 0;
	
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
			{
				std::cerr << "Could not parse IP address: " << ip_str << " defaulting to INADDR_ANY" << std::endl;
				this->ip = INADDR_ANY;
				return ;
			}
			ip_uint += octets[i] * pow(2, (3 - i) * 8);
        }
	}
	this->ip = htonl(ip_uint);
}

void		Config::parseDefaultErrorPages(std::istringstream &conf_stream)
{
	std::string			newline;
	int					field;
	std::string			equal, value;

	while (std::getline(conf_stream, newline))
	{
		if (utils::stringTrim(newline, " \t").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t") == "end")
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> field >> equal >> value) || (equal != "=" && equal != ":"))
		{
			std::cerr << "Could not parse config line " << newline << std::endl;
			continue;
		}
		if (field < 200 || field > 600)
		{
			std::cerr << "Unknown error number " << field << " in Default Error Page définitions" << std::endl;
			continue;
		}
		this->default_error_pages[field] = value;
	}
}

void	Config::parseAccept(std::istringstream &conf_stream)
{
	std::string			newline;
	std::string			field;

	while (std::getline(conf_stream, newline))
	{
		if (utils::stringTrim(newline, " \t").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t") == "end")
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> field))
		{
			std::cerr << "Could not parse config line " << newline << std::endl;
			continue;
		}
		if (this->strToContentType(field) == FTYPE_NONE)
		{
			std::cerr << "Could not interpret Accept section in config: " << newline << std::endl;
			continue;
		}
		this->accept_list.push_back(this->strToContentType(field));
	}
}

void		Config::parseHttpRedir(std::istringstream &conf_stream)
{
	std::string			newline;
	std::string			path, equal, dest;
	int					error_code;

	while (std::getline(conf_stream, newline))
	{
		if (utils::stringTrim(newline, " \t").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t") == "end")
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> path >> equal >> dest >> error_code) || (equal != "=" && equal != ":"))
		{
			std::cerr << "Could not parse config line " << newline << std::endl;
			continue;
		}
		this->http_redir[path].dest = dest;
		this->http_redir[path].error_code = error_code;
		if (newline.find("}") != std::string::npos)
			return ;
	}
}

ContentTypes		Config::strToContentType(std::string input)
{
	if (input == "*/*")
		return (FTYPE_ANY);
	if (input == "text/*")
		return (FTYPE_TEXT);
	if (input == "text/plain")
		return (FTYPE_PLAIN);
	if (input == "image/*")
		return (FTYPE_IMAGE);
	if (input == "text/html")
		return (FTYPE_HTML);
	if (input == "image/png")
		return (FTYPE_PNG);
	if (input == "image/jpeg")
		return (FTYPE_JPEG);
	return (FTYPE_NONE);
}

std::ostream	&operator<<(std::ostream &os, const Config &item)
{
	os << "IP :" << item.ip << std::endl;
	os << "port_number :" << item.port_number << std::endl;
	os << "incoming_queue_backlog :" << item.incoming_queue_backlog << std::endl;
	os << "client_limit :" << item.client_limit << std::endl;
	os << "domain :" << item.domain << std::endl;
	os << "type :" << item.type << std::endl;
	os << "protocol :" << item.protocol << std::endl;
	os << "buffer_size :" << item.buffer_size << std::endl;
	os << "ServHome :" << item.server_home << std::endl;
	os << "default_page :" << item.default_page << std::endl;
	os << "parse_error :" << item.parse_error << std::endl;
	os << "cookie_life_time :" << item.cookie_life_time << std::endl;
	os << "cookie_sessions_max :" << item.cookie_sessions_max << std::endl;
	return (os);
}
