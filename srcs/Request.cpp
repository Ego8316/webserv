/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hcavet <hcavet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:12:49 by ego               #+#    #+#             */
/*   Updated: 2025/12/04 23:01:25 by hcavet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/**
 * @brief Initializes an empty request with default values.
 */
Request::Request()
{
	this->_raw_header = "";
	this->_raw_body = "";
	this->_method = UNKNOWN;
	this->_request_target = "";
	this->_query_string = "";
	this->_version = "";
	this->_content_length = 0;
	this->_chunked = false;
	this->_headers = std::map<std::string, std::string>();
	this->_error = false;
	this->_accept = FTYPE_NONE;
	this->_query_cookies = NULL;
	return ;
}

/**
 * @brief Copy constructor.
 *
 * @param other Source request.
 */
Request::Request(const Request &other)
{
	*this = other;
	return ;
}

/**
 * @brief Assignment operator.
 *
 * @param other Source request.
 *
 * @return Reference to this request.
 */
Request &Request::operator=(const Request &other)
{
	if (this != &other)
	{
		this->_raw_header = other._raw_header;
		this->_raw_body = other._raw_body;
		this->_method = other._method;
		this->_request_target = other._request_target;
		this->_query_string = other._query_string;
		this->_version = other._version;
		this->_content_length = other._content_length;
		this->_chunked = other._chunked;
		this->_headers = other._headers;
		this->_error = other._error;
		this->_accept = other._accept;
		if (this->_query_cookies)
			delete this->_query_cookies;
		this->_query_cookies = new Cookie(*other._query_cookies);
	}
	return (*this);
}

/**
 * @brief Destructor. Frees any allocated cookies.
 */
Request::~Request()
{
	delete this->_query_cookies;
	return ;
}

/**
 * @brief Parses the raw header into method, target, version, and headers.
 *
 * @param config Server configuration for size limits and defaults.
 */
void	Request::parseHeader(const ServerConfig &config)
{
	std::istringstream	stream(this->_raw_header);
	std::string			line;
	std::string			method_str;

	if (!std::getline(stream, line))
	{
		this->_error = true;
		return ;
	}
	std::istringstream	first_line(utils::stringTrim(line, "\r\n \t"));
	if (!(first_line >> method_str >> this->_request_target >> this->_version))
	{
		this->_error = true;
		return ;
	}
	if (!first_line.eof())
	{
		this->_error = true;
		return ;
	}
	this->_method = utils::strToMethod(method_str);
	this->_parseRequestTarget();
	while (std::getline(stream, line))
	{
		line = utils::stringTrim(line, "\r\n \t");
		if (line.empty())
		{
			this->_error = true;
			return ;
		}
		_parseHeaderLine(line, config);
		if (this->_error)
			return ;
	}
	if (_chunked && _content_length)
	{
		this->_error = true;
		return ;
	}
	if (this->_accept == FTYPE_NONE)
		this->_accept = FTYPE_ANY;
	if (this->_query_cookies == NULL)
		this->_query_cookies = new Cookie();
	return ;
}

/**
 * @brief Extracts query string and strips scheme/host prefixes from target.
 */
void	Request::_parseRequestTarget()
{
	if (this->_request_target.find("?") != std::string::npos)
	{
		this->_query_string = this->_request_target.substr(this->_request_target.find("?") + 1, this->_request_target.length());
		this->_request_target.erase(this->_request_target.find("?"), this->_request_target.length());
	}
	if (utils::startsWith(this->_request_target, "http://"))
		this->_request_target.erase(0, 8);
	if (utils::startsWith(this->_request_target, "www"))
		this->_request_target.erase(0, this->_request_target.find("/"));
	return ;
}

/**
 * @brief Parses a single header line and updates internal state.
 *
 * @param line Header line without CRLF.
 * @param config Server configuration for size limits.
 */
void		Request::_parseHeaderLine(std::string line, const ServerConfig &config)
{
	std::vector<std::string>	field_split = utils::stringSplit(line, ": ");
	Cookie						*cookie;
	
	if (field_split.size() == 2)
	{
		std::string	key = utils::capitalize(field_split[0]);
		std::string	value = field_split[1];
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);
		if (!value.empty() && value[value.size() - 1] == '\r')
			value.erase(value.size() - 1);
		if (key == "Cookie")
		{
			cookie = new Cookie(value);
			if (cookie && cookie->applyToPath(_request_target))
			{
				if (this->_query_cookies)
					delete this->_query_cookies;
				this->_query_cookies = cookie;
			}
			else
				delete cookie;
		}
		else if (key == "Accept")
			this->_accept = static_cast<ContentType>(this->_accept | utils::strToContentType(value));
		else if (headerHasField(key))
			this->_headers[key] = _headers[key] + "; " + value;
		else
			this->_headers[key] = value;
		if (key == "Transfer-Encoding" && utils::toLower(value).find("chunked") != std::string::npos)
			this->_chunked = true;
		if (this->getMethod() == GET && key == "Content-Length")
			this->_error = true;
		else if (key == "Content-Length")
		{
			this->_content_length = std::atol(value.c_str());
			if (this->_content_length > config.client_max_body_size)
			{
				this->_error = true;
				return ;
			}
		}
	}
	else
		this->_error = true;
	return ;
}

/**
 * @brief Returns a mutable reference to the raw header buffer.
 *
 * @return Reference to raw header string.
 */
std::string	&Request::getRawHeader()
{
	return (this->_raw_header);
}

/**
 * @brief Returns a mutable reference to the raw body buffer.
 *
 * @return Reference to raw body string.
 */
std::string &Request::getRawBody()
{
	return (this->_raw_body);
}

/**
 * @brief Returns a const reference to the raw body buffer.
 *
 * @return Const reference to raw body string.
 */
const std::string &Request::getRawBody() const
{
	return (this->_raw_body);
}

/**
 * @brief Returns the parsed HTTP method.
 *
 * @return Method enum value.
 */
Method	Request::getMethod() const
{
	return (this->_method);
}

/**
 * @brief Returns the normalized request target without query string.
 *
 * @return Request target path.
 */
std::string	Request::getRequestTarget() const
{
	return (this->_request_target);
}

/**
 * @brief Returns the HTTP version string.
 *
 * @return HTTP version.
 */
std::string Request::getVersion() const
{
	return (this->_version);
}

/**
 * @brief Returns Content-Length value (0 if missing).
 *
 * @return Declared content length.
 */
size_t	Request::getContentLength() const
{
	return (this->_content_length);
}

/**
 * @brief Indicates whether Transfer-Encoding included chunked.
 *
 * @return True when chunked.
 */
bool	Request::isChunked() const
{
	return (this->_chunked);
}

/**
 * @brief Returns a copy of parsed headers.
 *
 * @return Headers map.
 */
const std::map<std::string, std::string>	&Request::getHeaders(void) const
{
	return (this->_headers);
}

/**
 * @brief True when parsing failed.
 *
 * @return Error flag.
 */
bool	Request::getError() const
{
	return (this->_error);
}

/**
 * @brief Returns parsed cookies, allocating an empty one if needed.
 *
 * @return Reference to query cookies object.
 */
const Cookie	&Request::getQueryCookies()
{
	if (!this->_query_cookies)
		this->_query_cookies = new Cookie();
	return (*this->_query_cookies);
}

/**
 * @brief Returns aggregated Accept content types.
 *
 * @return Accept bitmask.
 */
ContentType	Request::getAccept() const
{
	return (this->_accept);
}

/**
 * @brief Returns the raw query string without leading '?'.
 *
 * @return Query string.
 */
std::string	Request::getQueryString() const
{
	return (this->_query_string);
}

/**
 * @brief Converts a chunked-encoded body to a raw body buffer.
 */
void	Request::unchunkBody()
{
	size_t				pos;
	size_t				next_nl;
	const char			*hexalen;
	long				len = 1;
	std::string			chunk;
	std::string			unchunked;
	char				*endPtr;
	
	pos = 0;
	while (len)
	{
		next_nl = this->_raw_body.find("\r\n", pos);
		hexalen = this->_raw_body.substr(pos, next_nl).c_str();
		pos = next_nl + 2;
		len = strtol(hexalen, &endPtr, 16);
		if (*endPtr != '\0' || len < 0)
		{
			std::cerr << "Cannot recogonize chunk size" << std::endl;
			return ;
		}
		pos = next_nl + 2;
		next_nl = this->_raw_body.find("\r\n", pos);
		if (pos + len != next_nl)
		{
			std::cerr << "Wrong chunk size" << std::endl;
			return ;
		}
		unchunked += this->_raw_body.substr(pos, next_nl);
	}
	this->_raw_body = unchunked;
	return ;
}

void	Request::setMethod(Method method)
{
	this->_method = method;
}

/**
 * @brief Marks the request as erroneous or valid.
 *
 * @param error Error flag.
 */
void	Request::setError(bool error)
{
	this->_error = error;
}

/**
 * @brief Checks if a header field exists.
 *
 * @param field Header name.
 *
 * @return True when present.
 */
bool	Request::headerHasField(const std::string field)
{
	return (this->_headers.find(field) != this->_headers.end());
}

/**
 * @brief Returns a header value or an empty string when missing.
 *
 * @param field Header name.
 *
 * @return Header value or empty string.
 */
std::string	Request::headerGetField(const std::string field)
{
	if (this->_headers.find(field) != this->_headers.end())
		return (this->_headers[field]);
	return ("");
}

/**
 * @brief Stream insertion for debugging request state.
 *
 * @param os Output stream.
 * @param src Request to render.
 *
 * @return Reference to output stream.
 */
std::ostream	&operator<<(std::ostream &os, const Request &src)
{
	Method										method = src.getMethod();
	const std::map<std::string, std::string>	&headers = src.getHeaders();
	std::string									methodStr;

	if (src.getError())
		os << "BAD REQUEST" << std::endl;
	else
		os << "GOOD REQUEST" << std::endl;
	methodStr = utils::methodToStr(method);
	os << "Method:\t\t" << methodStr << std::endl
		<< "Target:\t\t" << src.getRequestTarget() << std::endl
		<< "Version:\t" << src.getVersion() << std::endl
		<< "Headers:\t" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		os << "\t\t" << it->first << ": " << it->second << std::endl;
	os << "Body:\n" << src.getRawBody() << std::endl;
	return (os);
}
