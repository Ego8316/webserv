/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:22:55 by ego               #+#    #+#             */
/*   Updated: 2025/10/23 15:10:48 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "Response.hpp"
#include "Cookie.hpp"
#include "Resource.hpp"
#include "Request.hpp"
#include "CGI.hpp"

class	Response;
class	Config;
class	Client;
class	Cookie;
class	Request;
class	Resource;

class	RequestHandler
{
	public:
		static void	handle(Response *response, const Request &request, const Config &config, const Cookie &cookies);

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
