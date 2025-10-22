/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:22:55 by ego               #+#    #+#             */
/*   Updated: 2025/10/22 12:02:02 by victorviter      ###   ########.fr       */
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
		static Response	handle(const Request &request, const Config &config, const Cookie &cookies);

	private:
		RequestHandler();
		RequestHandler(const RequestHandler &other);
		RequestHandler &operator=(const RequestHandler &other);
		~RequestHandler();

		static Response	_handleGet(const Request &request, const Config &config, const Resource &resource);
		static Response	_handlePost(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleDelete(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleRedirect(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleListDir(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleError(HttpStatus code, const Config &config);
		static Response	_handleCGI();
};
