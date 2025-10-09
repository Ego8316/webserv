/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:33:19 by ego               #+#    #+#             */
/*   Updated: 2025/10/08 22:18:40 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

RequestHandler::RequestHandler(void)
	:	_config(NULL),
		_client(NULL),
		_request(NULL)
{
	return ;
}

RequestHandler::RequestHandler(const Config *config, const Client *client)
	:	_config(config),
		_client(client),
		_request(NULL)
{
	return ;
}

RequestHandler::RequestHandler(const RequestHandler &other)
	:	_config(other._config),
		_client(other._client),
		_request(other._request)
{
	return ;
}

RequestHandler	&RequestHandler::operator=(const RequestHandler &other)
{
	if (this != &other)
	{
		_config = other._config;
		_client = other._client;
		_request = other._request;
	}
	return (*this);
}

RequestHandler::~RequestHandler(void)
{
	return ;
}

