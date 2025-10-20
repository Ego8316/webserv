/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:30:36 by ego               #+#    #+#             */
/*   Updated: 2025/10/20 20:35:06 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

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
		std::string			toString(void) const;

		static std::string	getDefaultErrorPage(HttpStatus code);
};
