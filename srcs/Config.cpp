/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 12:13:25 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config() {}

Config::Config(std::string filename)
{
	(void)filename;
	this->port_number = 12345;
	this->time_out = NO_TIMEOUT;
	this->domain = AF_INET;
	this->type = SOCK_STREAM;
	this->protocol = 0;
	this->client_limit = CLIENT_LIMIT;
	this->buffer_size = BUFFER_SIZE;
	this->ServHome = SERVER_HOME;
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