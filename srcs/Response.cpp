/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:35:57 by ego               #+#    #+#             */
/*   Updated: 2025/10/13 16:11:09 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/**
 * @brief Default constructor. Initializes status code to 200 OK.
 */
Response::Response(void)
	:	_status_code(HTTP_OK)
{
	return ;
}

/**
 * @brief Copy constructor.
 * @param other The Response object to copy.
 */
Response::Response(const Response &other)
{
	*this = other;
	return ;
}

/**
 * @brief Assignment operator.
 * @param other The Response object to assign from.
 * @return Reference to this Response object.
 */
Response	&Response::operator=(const Response &other)
{
	if (this != &other)
	{
		_status_code = other._status_code;
		_header = other._header;
		_body = other._body;
		_headers = other._headers;
	}
	return (*this);
}

/**
 * @brief Destructor.
 */
Response::~Response(void)
{
	return ;
}

/**
 * @brief Sets the HTTP status code for the response.
 * @param code HTTP status code (e.g., OK, HTTP_NOT_FOUND).
 */
void	Response::setStatus(HttpStatus code)
{
	_status_code = code;
	return ;
}

/**
 * @brief Sets the response body content.
 * @param body The body as a string.
 */
void	Response::setBody(const std::string &body)
{
	_body = body;
	return ;
}

/**
 * @brief Sets or updates a header field.
 * @param key Header name.
 * @param value Header value.
 */
void	Response::setHeaders(const std::string &key, const std::string &value)
{
	_headers[key] = value;
	return ;
}

/**
 * @brief Sets or updates the content type header field.
 * @param type Content type.
 */
void	Response::setContentType(const std::string &type)
{
	_headers["Content-Type"] = type;
	return ;
}

/**
 * @brief Sets or updates the content length header field.
 * @param type Content length.
 */
void	Response::setContentLength(size_t len)
{
	_headers["Content-Length"] = utils::toString(len);
	return ;
}

/**
 * @brief Sets or updates the cookie header field.
 * @param type Cookie.
 */
void	Response::setCookie(const std::string &cookie)
{
	_headers["Set-Cookie"] = cookie;
	return ;
}

/**
 * @brief Converts an HTTP status code enum to its corresponding string.
 * @param code The HTTP status code.
 * @return `std::string` The string representation of the code.
 */
std::string	Response::httpStatusToStr(HttpStatus code)
{
	switch(code)
	{
		case HTTP_OK:						return "OK";
		case HTTP_BAD_REQUEST:				return "Bad Request";
		case HTTP_FORBIDDEN:				return "Forbidden";
		case HTTP_NOT_FOUND:				return "Not Found";
		case HTTP_INTERNAL_SERVER_ERROR:	return "Internal Server Error";
		case HTTP_NOT_IMPLEMENTED:			return "Not Implemented";
		case HTTP_VERSION_NOT_SUPPORTED:	return "HTTP Version Not Supported";
		default:							return "Unknown";
	}
}

/**
 * @brief Builds the HTTP response header string from the status code and
 * currently set headers.
 * 
 * Automatically adds default headers ("Server" and "Connection") if they are
 * not already set. The resulting string is stored in the _header member.
 */
void	Response::buildHeader(void)
{
	if (!utils::mapHasEntry(_headers, std::string("Server")))
		_headers["Server"] = "Webserv/1.0 (Unix)";
	if (!utils::mapHasEntry(_headers, std::string("Connection")))
		_headers["Connection"] = "close";

	_header = "HTTP/1.0 " + utils::toString(_status_code)
		+ " " + httpStatusToStr(_status_code) + "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		_header += it->first + ": " + it->second + "\r\n";
	_header += "\r\n";
	return ;
}

/**
 * @brief Returns the built HTTP header string.
 * @return Reference to the header string.
 */
const std::string	&Response::getHeader(void) const
{
	return (_header);
}

/**
 * @brief Returns the body string.
 * @return Reference to the body string.
 */
const std::string	&Response::getBody(void) const
{
	return (_body);
}

/**
 * @brief Returns the full HTTP response (header + body) as a string.
 * @return Full HTTP response string.
 */
std::string	Response::toString(void) const
{
	return (_header + _body);
}

/**
 * @brief Returns a default error page in case the server configuration
 * does not give one.
 * @param code HTTP status code.
 * @return HTML code for the error page.
 */
std::string	Response::getDefaultErrorPage(HttpStatus code)
{
	switch(code)
	{
		case HTTP_BAD_REQUEST:				return ERROR_PAGE_400;
		case HTTP_FORBIDDEN:				return ERROR_PAGE_403;
		case HTTP_NOT_FOUND:				return ERROR_PAGE_404;
		case HTTP_INTERNAL_SERVER_ERROR:	return ERROR_PAGE_500;
		case HTTP_NOT_IMPLEMENTED:			return ERROR_PAGE_501;
		case HTTP_VERSION_NOT_SUPPORTED:	return ERROR_PAGE_505;
		default:							return ERROR_PAGE_500;
	}
}

/**
 * @brief Stream insertion operator for Response.
 *
 * Writes the full HTTP response (header + body) to an output stream.
 *
 * @param os The output stream to write to.
 * @param src The Response object to output.
 * @return `std::ostream&` Reference to the output stream.
 */
std::ostream	&operator<<(std::ostream &os, const Response &src)
{
	os << src.getHeader() << src.getBody();
	return (os);
}
