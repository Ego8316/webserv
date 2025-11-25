/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:30:36 by ego               #+#    #+#             */
/*   Updated: 2025/11/24 23:41:02 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "CGI.hpp"

class	CGI;

/**
 * @class Response
 *
 * @brief Represents an HTTP response.
 *
 * Stores the HTTP status code, headers and body. Provides utility functions to
 * build the full HTTP response string to be sent to a client.
 */
class	Response
{
	public:
		Response();
		Response(const Response &other);
		Response	&operator=(const Response &other);
		~Response();

		void	setStatus(HttpStatus code);
		void	setBody(const std::string &body);
		void	setHeaders(const std::string &key, const std::string &value);
		void	setCGI(CGI *cgi);
		void	setFd(int fd);

		void	setContentType(const std::string &type);
		void	setContentLength(size_t len);
		void	setCookie(const std::string &cookie);

		void	buildHeader();
		void	build();

		const std::string	&getHeader() const;
		const std::string	&getBody() const;
		const std::string	&getString() const;
		CGI					*getCGI();
		bool				isCGI();
		int					getFd() const;
		HttpStatus			getHttpStatus() const;

		static std::string	getDefaultErrorPage(HttpStatus code);

	private:
		HttpStatus							_status_code;
		std::string							_header;
		std::string							_body;
		std::string							_string;
		std::map<std::string, std::string>	_headers;
		CGI									*_cgi;
		bool								_is_cgi;
		int									_body_fd;
};

std::ostream	&operator<<(std::ostream &os, const Response &src);
