/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:22:55 by ego               #+#    #+#             */
/*   Updated: 2025/10/20 19:40:25 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

#include "Cookie.hpp"
#include "Resource.hpp"
#include "Response.hpp"
#include "Request.hpp"

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
		RequestHandler(void);
		RequestHandler(const RequestHandler &other);
		RequestHandler &operator=(const RequestHandler &other);
		~RequestHandler(void);

		static Response	_handleGet(const Request &request, const Config &config, const Resource &resource);
		static Response	_handlePost(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleDelete(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleCGI(const Request &request, const Config &config, const Resource &resource, const Cookie &cookies);
		static Response	_handleRedirect(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleListDir(const Request &request, const Config &config, const Resource &resource);
		static Response	_handleError(HttpStatus code, const Config &config);
};
