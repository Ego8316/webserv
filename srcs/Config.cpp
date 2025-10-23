/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/23 16:44:54 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(std::string config)
{
	std::istringstream	conf_stream(config);
	std::string			newline;
	std::string			field, equal, value;

	while (std::getline(conf_stream, newline))
	{
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue ;
		std::istringstream 			line(newline);
	
		if (!(line >> field >> equal >> value) || (equal != "=" && equal != ":"))
		{
			std::cerr << "Could not parse config line " << newline << " in global"   << std::endl;
			continue;
		}
		if (field == "IP")
			this->setIP(value);
		if (field == "PORT")
			this->port_number = atoi(value.c_str());
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
		else if (field == "METHODS")
			this->parseMethod(conf_stream);
		else if (field == "MAX_HEADER_SIZE")
			this->max_header_size = atoi(value.c_str());
		else if (field == "MAX_BODY_SIZE")
			this->max_body_size = atoi(value.c_str());
		else if (field == "CLIENT_LIMIT")
			this->client_limit = atoi(value.c_str());
		else if (field == "PROCESSING_TIME_LIMIT")
			this->processing_time_limit = atoi(value.c_str());
		else if (field == "MAX_REQUEST_TIME")
			this->max_request_time = atoi(value.c_str());
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
		else if (field == "HTTP_REDIR")
			this->parseHttpRedir(conf_stream);
	}
}

Config::Config(const Config &other)
{
	*this = other;
}

Config &Config::operator=(const Config &other)
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

std::map<std::string, Redirection>		Config::getRedirections() const
{
	return (this->http_redir);
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
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t\n") == "end")
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> field >> equal >> value) || (equal != "=" && equal != ":"))
		{
			std::cerr << "Could not parse config line " << newline << " in default error pages" << std::endl;
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

void	Config::parseMethod(std::istringstream &conf_stream)
{
	std::string			newline;
	std::string			field;

	while (std::getline(conf_stream, newline))
	{
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t\n") == "end")
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> field))
		{
			std::cerr << "Could not parse config line " << newline << " in methods"  << std::endl;
			continue;
		}
		if (utils::strToMethod(field) == UNKNOWN)
		{
			std::cerr << "Could not interpret methods section in config: " << newline << std::endl;
			continue;
		}
		this->accepted_methods.push_back(utils::strToMethod(field));
	}
}

void		Config::parseHttpRedir(std::istringstream &conf_stream)
{
	std::string			newline;
	std::string			path, equal, dest;
	int					error_code;

	while (std::getline(conf_stream, newline))
	{
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t\n") == "end")
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> path >> equal >> dest >> error_code) || (equal != "=" && equal != ":"))
		{
			std::cerr << "Could not parse config line " << newline << " in http redirections"  << std::endl;
			continue;
		}
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
	{
		if (element == this->accepted_methods[i])
			return (true);
	}
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
