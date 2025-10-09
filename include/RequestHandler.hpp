/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:22:55 by ego               #+#    #+#             */
/*   Updated: 2025/10/09 18:24:20 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

class	Config;
class	Client;
class	Cookie;
class	Request;
class	Response;

class	RequestHandler
{
	public:
		RequestHandler(void);
		RequestHandler(const Config *config, const Client *client);
		RequestHandler(const RequestHandler &other);
		RequestHandler &operator=(const RequestHandler &other);
		~RequestHandler(void);

	private:
		const Config	*_config;
		const Client	*_client;
		Request			*_request;

		Response	handleGet(const Request &req);
		Response	handlePost(const Request &req);
		Response	handleDelete(const Request &req);
		Response	handleCGI(const Request &req);
		Response	handleError(HttpStatus code);

		std::string	findResource(const std::string &target);
};
