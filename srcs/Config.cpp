/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/29 18:51:02 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

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
}

Config &Config::operator=(const Config &other)
{
}

Config::~Config() {}


ParseError		Config::getParseError()
{
	return (this->parse_error);
}