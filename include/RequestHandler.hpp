/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:22:55 by ego               #+#    #+#             */
/*   Updated: 2025/10/14 12:30:24 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

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
		static Response	handle(const Request &reqest, const Config &config, const Cookie &cookies);

	private:
		RequestHandler(void);
		RequestHandler(const RequestHandler &other);
		RequestHandler &operator=(const RequestHandler &other);
		~RequestHandler(void);

		static Response	_handleGet(const Request &reqest, const Config &config, const Resource &resource);
		static Response	_handlePost(const Request &reqest, const Config &config, const Resource &resource);
		static Response	_handleDelete(const Request &reqest, const Config &config, const Resource &resource);
		static Response	_handleCGI(const Request &reqest, const Config &config, const Resource &resource, const Cookie &cookies);
		static Response	_handleRedirect(const Request &reqest, const Config &config, const Resource &resource);
		static Response	_handleListDir(const Request &reqest, const Config &config, const Resource &resource);
		static Response	_handleError(HttpStatus code, const Config &config);
};
