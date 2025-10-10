/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:22:55 by ego               #+#    #+#             */
/*   Updated: 2025/10/10 11:15:46 by victorviter      ###   ########.fr       */
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

class	RequestHandler
{
	public:
		static Response	handle(const Request &req, const Config &config, std::vector<Cookie *> cookies);

	private:
		RequestHandler(void);
		RequestHandler(const RequestHandler &other);
		RequestHandler &operator=(const RequestHandler &other);
		~RequestHandler(void);

		static Response	_handleGet(const Request &req, const Config &config, const Resource &res);
		static Response	_handlePost(const Request &req, const Config &config, const Resource &res);
		static Response	_handleDelete(const Request &req, const Config &config, const Resource &res);
		static Response	_handleCGI(const Request &req, const Config &config, const Resource &res);
		static Response	_handleRedirect(const Request &req, const Config &config, const Resource &res);
		static Response	_handleListDir(const Request &req, const Config &config, const Resource &res);
		static Response	_handleError(HttpStatus code, const Config &config);
};
