/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:12:40 by ego               #+#    #+#             */
/*   Updated: 2025/10/02 13:22:03 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Cookie.hpp"
#include "utils.hpp"
#include "Config.hpp"

class Cookie;

class Request
{
	public:
		//Request();
		Request(Config *config);
		Request(const Request &other);
		Request	&operator=(const Request &other);
		~Request(void);

		int									parseRequest(std::string request);
		int									parseHeaderLine(std::string line);
		Method								getMethod(void) const;
		std::string							getRequestTarget(void) const;
		std::string							getVersion(void) const;
		std::string							getRawBody(void) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		int									getError(void) const;
		Cookie								*getCookie(void) const;

		void								setMethod(Method method);
		int									setCookie();
		bool								headerHasField(const std::string field);
		std::string							headerGetField(const std::string field);

	private:
		Method								_method;
		std::string							_requestTarget;
		std::string							_version;
		std::string							_rawBody;
		Cookie								*_cookie;
		std::map<std::string, std::string>	_headers;
		int									_error;
		Config								*_config;
};

std::ostream	&operator<<(std::ostream &os, const Request &src);
