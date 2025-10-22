/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:30:36 by ego               #+#    #+#             */
/*   Updated: 2025/10/22 12:00:22 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "CGI.hpp"

class	CGI;

/**
 * @class Response
 * @brief Represents an HTTP response.
 * 
 * Stores the HTTP status code, headers and body. Provides utility functions to
 * build the full HTTP response string to be sent to a client.
 */
class	Response
{
	private:
		HttpStatus							_status_code;
		std::string							_header;
		std::string							_body;
		std::map<std::string, std::string>	_headers;
		CGI									*_cgi;
		bool								_is_cgi;
		
	public:
		Response(void);
		Response(const Response &other);
		Response	&operator=(const Response &other);
		~Response(void);

		void	setStatus(HttpStatus code);
		void	setBody(const std::string &body);
		void	setHeaders(const std::string &key, const std::string &value);

		void	setContentType(const std::string &type);
		void	setContentLength(size_t len);
		void	setCookie(const std::string &cookie);

		void		buildHeader(void);

		const std::string	&getHeader(void) const;
		const std::string	&getBody(void) const;
		CGI					*getCGI();
		void				setCGI(CGI *cgi);
		bool				isCGI();
		std::string			toString(void) const;

		static std::string	getDefaultErrorPage(HttpStatus code);
};
