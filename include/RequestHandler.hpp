/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:22:55 by ego               #+#    #+#             */
/*   Updated: 2025/12/05 02:52:36 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "Response.hpp"
#include "Resource.hpp"
#include "Request.hpp"
#include "CGI.hpp"
#include "ServerConfig.hpp"

class	Response;
class	ServerConfig;
class	Client;
class	Request;
class	Resource;

/**
 * @class RequestHandler
 *
 * @brief Static dispatcher that builds HTTP responses from parsed requests.
 */
class	RequestHandler
{
	public:
		static void	handle(Response *response, const Request &request, const ServerConfig &config);
		static void	_handleError(Response *response, HttpStatus code, const ServerConfig &config);

	private:
		RequestHandler();
		RequestHandler(const RequestHandler &other);
		RequestHandler &operator=(const RequestHandler &other);
		~RequestHandler();

		static void	_handleGet(Response *response, const ServerConfig &config, const Resource &resource);
		static void	_handlePost(Response *response, const Request &request, const ServerConfig &config, const Resource &resource);
		static void	_handleDelete(Response *response, const ServerConfig &config, const Resource &resource);
		static void	_handleRedirect(Response *response, const Resource &resource);
		static void	_handleListDir(Response *response, const ServerConfig &config, const Resource &resource);
		static void	_handleCGI(Response *response, const ServerConfig &config, const Resource &resource);
		static void	_handleErrno(Response *response, const ServerConfig &config);
};
