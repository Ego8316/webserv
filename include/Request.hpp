/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:12:40 by ego               #+#    #+#             */
/*   Updated: 2025/10/13 16:33:35 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Config.hpp"
#include "utils.hpp"
#include "Cookie.hpp"

class Cookie;

class Request
{
	public:
		Request(std::map<std::string, Cookie *> *all_cookies);
		Request(const Request &other);
		Request	&operator=(const Request &other);
		~Request(void);

		int									parseRequest(std::string request, const Config &config);
		int									parseHeaderLine(std::string line);
		Method								getMethod(void) const;
		std::string							getRequestTarget(void) const;
		std::string							getVersion(void) const;
		std::string							getRawBody(void) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		bool								getError(void) const;
		std::vector<Cookie *>				getQueryCookies();
		ContentTypes						getAccept() const;

		void								setMethod(Method method);
		bool								headerHasField(const std::string field);
		std::string							headerGetField(const std::string field);

	private:
		Method								_method;
		std::string							_requestTarget;
		std::string							_version;
		std::string							_rawBody;
		std::map<std::string, std::string>	_headers;
		bool								_error;
		ContentTypes						_accept;
		std::map<std::string, Cookie *>		*_all_cookies;
		std::vector<Cookie *>				_query_cookies; //TODO
};

std::ostream	&operator<<(std::ostream &os, const Request &src);
