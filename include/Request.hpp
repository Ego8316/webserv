/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:12:40 by ego               #+#    #+#             */
/*   Updated: 2025/09/29 18:30:46 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Cookie.hpp"

class Request
{
	public:
		Request(void);
		Request(const Request &other);
		Request(const std::string &raw);
		Request	&operator=(const Request &other);
		~Request(void);

		Method								getMethod(void) const;
		std::string							getRequestTarget(void) const;
		std::string							getVersion(void) const;
		std::string							getRawBody(void) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		int									getError(void) const;

		void								setMethod(Method method);
		int									updateCookie();
		bool								headerHasField(const std::string field);
		std::string							headerGetField(const std::string field);

	private:
		Method								_method;
		std::string							_requestTarget;
		std::string							_version;
		std::string							_rawBody;
		Cookie								_cookie;
		std::map<std::string, std::string>	_headers;
		int									_error;
};

std::ostream	&operator<<(std::ostream &os, const Request &src);
