/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:12:40 by ego               #+#    #+#             */
/*   Updated: 2025/10/23 15:14:14 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "Config.hpp"
#include "Cookie.hpp"

class	Config;	
class	Cookie;

class	Request
{
	public:
		Request();
		Request(const Request &other);
		Request	&operator=(const Request &other);
		~Request();

		int		parseHeader(const Config &config);

		std::string							&getRawHeader();
		std::string							&getRawBody();
		const std::string					&getRawBody() const;
		Method								getMethod() const;
		std::string							getRequestTarget() const;
		std::string							getVersion() const;
		size_t								getContentLength() const;
		bool								isChunked() const;
		std::map<std::string, std::string>	getHeaders() const;
		bool								getError() const;
		const Cookie						&getQueryCookies();
		ContentType						getAccept() const;
		std::string							getQueryString() const;
		void								unchunkBody();

		void								setMethod(Method method);
		void								setError(bool error);
		bool								headerHasField(const std::string field);
		std::string							headerGetField(const std::string field);

	private:
		std::string							_raw_header;
		std::string							_raw_body;
		Method								_method;
		std::string							_request_target;
		std::string							_query_string;
		std::string							_version;
		size_t								_content_length;
		bool								_chunked;
		std::map<std::string, std::string>	_headers;
		bool								_error;
		ContentType						_accept;
		Cookie 								*_query_cookies;

		int		_parseRequestTarget();
		int		_parseHeaderLine(std::string line, const Config &config);
};

std::ostream	&operator<<(std::ostream &os, const Request &src);
