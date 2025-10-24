/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:22:55 by ego               #+#    #+#             */
/*   Updated: 2025/10/23 21:56:13 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "Response.hpp"
#include "Resource.hpp"
#include "Request.hpp"
#include "CGI.hpp"

class	Response;
class	Config;
class	Client;
class	Request;
class	Resource;

class	RequestHandler
{
	public:
		static void	handle(Response *response, const Request &request, const Config &config);

	private:
		RequestHandler();
		RequestHandler(const RequestHandler &other);
		RequestHandler &operator=(const RequestHandler &other);
		~RequestHandler();

		static void	_handleGet(Response *response, const Config &config, const Resource &resource);
		static void	_handlePost(Response *response, const Request &request, const Config &config, const Resource &resource);
		static void	_handleDelete(Response *response, const Config &config, const Resource &resource);
		static void	_handleRedirect(Response *response, const Resource &resource);
		static void	_handleListDir(Response *response, const Config &config, const Resource &resource);
		static void	_handleError(Response *response, HttpStatus code, const Config &config);
		static void	_handleCGI(Response *response);
};
