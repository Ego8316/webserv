/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:35:57 by ego               #+#    #+#             */
/*   Updated: 2025/10/23 02:27:11 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/**
 * @brief Default constructor. Initializes status code to 200 OK.
 */
Response::Response()
{
	this->_status_code = HTTP_OK;
	this->_header = "";
	this->_body = "";
	this->_cgi = NULL;
	this->_is_cgi = false;
	this->_body_fd = -1;
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
		this->_status_code = other._status_code;
		this->_header = other._header;
		this->_body = other._body;
		this->_headers = other._headers;
		if (this->_cgi)
			delete this->_cgi;
		this->_cgi = new CGI(*other._cgi);
		this->_is_cgi = other._is_cgi;
		this->_body_fd = other._body_fd;
	}
	return (*this);
}

/**
 * @brief Destructor.
 */
Response::~Response(void)
{
	if (this->_cgi)
	{
		delete this->_cgi;
		this->_cgi = NULL;
	}
	if (this->_body_fd > -1)
		close(this->_body_fd);
	return ;
}

/**
 * @brief Sets the HTTP status code for the response.
 * @param code HTTP status code (e.g., OK, HTTP_NOT_FOUND).
 */
void	Response::setStatus(HttpStatus code)
{
	this->_status_code = code;
	return ;
}

/**
 * @brief Sets the response body content.
 * @param body The body as a string.
 */
void	Response::setBody(const std::string &body)
{
	this->_body = body;
	return ;
}

/**
 * @brief Sets or updates a header field.
 * @param key Header name.
 * @param value Header value.
 */
void	Response::setHeaders(const std::string &key, const std::string &value)
{
	this->_headers[key] = value;
	return ;
}

void	Response::setCGI(CGI *cgi)
{
	this->_cgi = cgi;
	this->_is_cgi = true;
	return ;
}

void	Response::setFd(int fd)
{
	this->_body_fd = fd;
	return ;
}

/**
 * @brief Sets or updates the content type header field.
 * @param type Content type.
 */
void	Response::setContentType(const std::string &type)
{
	this->_headers["Content-Type"] = type;
	return ;
}

/**
 * @brief Sets or updates the content length header field.
 * @param type Content length.
 */
void	Response::setContentLength(size_t len)
{
	this->_headers["Content-Length"] = utils::toString(len);
	return ;
}

/**
 * @brief Sets or updates the cookie header field.
 * @param type Cookie.
 */
void	Response::setCookie(const std::string &cookie)
{
	this->_headers["Set-Cookie"] = cookie;
	return ;
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
	if (!utils::mapHasEntry(this->_headers, std::string("Server")))
		this->_headers["Server"] = "Webserv/1.0 (Unix)";
	if (!utils::mapHasEntry(_headers, std::string("Connection")))
		this->_headers["Connection"] = "close";
	this->_header = "HTTP/1.0 " + utils::toString(this->_status_code)
		+ " " + utils::httpStatusToStr(this->_status_code) + "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
		this->_header += it->first + ": " + it->second + "\r\n";
	if (!this->_is_cgi)
		this->_header += "\r\n";
	return ;
}

/**
 * @brief Returns the built HTTP header string.
 * @return Reference to the header string.
 */
const std::string	&Response::getHeader(void) const
{
	return (this->_header);
}

/**
 * @brief Returns the body string.
 * @return Reference to the body string.
 */
const std::string	&Response::getBody(void) const
{
	return (this->_body);
}

CGI	*Response::getCGI()
{
	return (this->_cgi);
}

bool	Response::isCGI()
{
	return (this->_is_cgi);
}


int	Response::getFd() const
{
	return (this->_body_fd);
}
		
/**
 * @brief Returns the full HTTP response (header + body) as a string.
 * @return Full HTTP response string.
 */
std::string	Response::toString(void) const
{
	return (this->_header + this->_body);
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
