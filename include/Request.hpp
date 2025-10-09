/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:12:40 by ego               #+#    #+#             */
/*   Updated: 2025/10/03 17:13:41 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "Config.hpp"

class Cookie;

class Request
{
	public:
		Request();
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

		void								setMethod(Method method);
		bool								headerHasField(const std::string field);
		std::string							headerGetField(const std::string field);

	private:
		Method								_method;
		std::string							_requestTarget;
		std::string							_version;
		std::string							_rawBody;
		std::map<std::string, std::string>	_headers;
		ParseError							_error;
};

std::ostream	&operator<<(std::ostream &os, const Request &src);
