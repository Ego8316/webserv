/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/02 17:53:11 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

//Config::Config() {}

Config::Config(std::string filename)
{
	std::ifstream 	conf_file;
	std::string		newline;

	conf_file.open(filename.c_str());// = open(filename.c_str(), O_RDONLY);
	if (!conf_file.is_open())
	{
		std::cerr << "Could not open config file" << std::endl;
		this->parse_error = UNREADABLE_FILE;
		//return (SERV_ERROR)
	}
	this->parse_error = NONE;
	while (std::getline(conf_file, newline))
	{
		std::vector<std::string>	split_str = stringSplit(newline, " = ");
		if (split_str.size() != 2)
			continue ;
		if (split_str[0] == "PORT_NUMBER")
			this->port_number = atoi(split_str[1].c_str());
		else if (split_str[0] == "TIME_OUT")
			this->time_out = atoi(split_str[1].c_str());
		else if (split_str[0] == "DOMAIN")
		{
			if (split_str[1] == "AF_INET")
				this->domain = AF_INET;
		}
		else if (split_str[0] == "TYPE")
		{
			if (split_str[1] == "SOCK_STREAM")
				this->type = SOCK_STREAM;
		}
		else if (split_str[0] == "PROTOCOL")
			this->protocol = atoi(split_str[1].c_str());
		else if (split_str[0] == "CLIENT_LIMIT")
			this->client_limit = atoi(split_str[1].c_str());
		else if (split_str[0] == "COOKIE_LIFE_TIME")
			this->cookie_life_time = atoi(split_str[1].c_str());
		else if (split_str[0] == "COOKIE_SESSIONS_MAX")
			this->cookie_sessions_max = atoi(split_str[1].c_str());
		else if (split_str[0] == "BUFFER_SIZE")
			this->buffer_size = atoi(split_str[1].c_str());
		else if (split_str[0] == "SERVER_HOME")
			this->server_home = split_str[1];
		else if (split_str[0] == "DEFAULT_PAGE")
			this->default_page = split_str[1];
		else if (split_str[0] == "INCOMMING_QUEUE_BACKLOG")
			this->incomming_queue_backlog = atoi(split_str[1].c_str());
	}
	conf_file.close();
	/*
	(void)filename;
	this->port_number = 12345;
	this->time_out = NO_TIMEOUT;
	this->domain = AF_INET;
	this->type = SOCK_STREAM;
	this->protocol = 0;
	this->client_limit = CLIENT_LIMIT;
	this->buffer_size = BUFFER_SIZE;
	this->ServHome = SERVER_HOME;
	*/
}

Config::Config(const Config &other)
{
	this->port_number = other.port_number;
	this->time_out = other.time_out;
	this->domain = other.domain;
	this->type = other.type;
	this->protocol = other.protocol;
	this->client_limit = other.client_limit;
	this->buffer_size = other.buffer_size;
	this->server_home = other.server_home;
}

Config &Config::operator=(const Config &other)
{
	if (this != &other)
	{
		this->port_number = other.port_number;
		this->time_out = other.time_out;
		this->domain = other.domain;
		this->type = other.type;
		this->protocol = other.protocol;
		this->client_limit = other.client_limit;
		this->buffer_size = other.buffer_size;
		this->server_home = other.server_home;
	}
	return (*this);
}

Config::~Config() {}


ParseError		Config::getParseError()
{
	return (this->parse_error);
}

std::ostream	&operator<<(std::ostream &os, const Config &item)
{
	os << "port_number :" << item.port_number << std::endl;
	os << "incomming_queue_backlog :" << item.incomming_queue_backlog << std::endl;
	os << "time_out :" << item.time_out << std::endl;
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