/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 22:39:39 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config() {}

Config::Config(std::string filename)
{
	std::ifstream 	conf_file;
	std::string		newline;

	conf_file.open(filename.c_str());// = open(filename.c_str(), O_RDONLY);
	if (!conf_file.is_open())
	{
		std::cerr << "Could not open config file" << std::endl;
		//return (SERV_ERROR)
	}
	while (std::getline(conf_file, newline))
	{
		std::vector<std::string>	split_str = stringSplit(newline, " = ");
		if (split_str.size() != 2)
			continue ;
		if (split_str[0].compare("PORT_NUMBER") == 0)
			this->port_number = atoi(split_str[1].c_str());
		else if (split_str[0].compare("TIME_OUT") == 0)
			this->time_out = atoi(split_str[1].c_str());
		else if (split_str[0].compare("DOMAIN") == 0)
		{
			if (split_str[1].compare("AF_INET") == 0)
				this->domain = AF_INET;
		}
		else if (split_str[0].compare("TYPE") == 0)
		{
			if (split_str[1].compare("SOCK_STREAM"))
				this->type = SOCK_STREAM;
		}
		else if (split_str[0].compare("PROTOCOL") == 0)
			this->protocol = atoi(split_str[1].c_str());
		else if (split_str[0].compare("CLIENT_LIMIT") == 0)
			this->client_limit = atoi(split_str[1].c_str());
		else if (split_str[0].compare("BUFFER_SIZE") == 0)
			this->buffer_size = atoi(split_str[1].c_str());
		else if (split_str[0].compare("SERVER_HOME") == 0)
			this->ServHome = split_str[1];
	}
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
	this->ServHome = other.ServHome;
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
		this->ServHome = other.ServHome;
	}
	return (*this);
}

Config::~Config() {}


ParseError		Config::getParseError()
{
	return (this->parse_error);
}