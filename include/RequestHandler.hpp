/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:22:55 by ego               #+#    #+#             */
/*   Updated: 2025/10/21 01:19:03 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "Response.hpp"
#include "Cookie.hpp"
#include "Resource.hpp"
#include "Request.hpp"

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
		static Response	_handleCGI(const Request &request, const Config &config, const Resource &resource, const Cookie &cookies);
		static Response	_handleRedirect(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleListDir(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleError(HttpStatus code, const Config &config);
};
