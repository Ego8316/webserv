/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:12:49 by ego               #+#    #+#             */
/*   Updated: 2025/10/20 19:53:26 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

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

Request::Request(const Request &other)
{
	*this = other;
	return ;
}

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
		this->_query_cookies = other._query_cookies;
	}
	return (*this);
}

Request::~Request()
{
	delete this->_query_cookies;
	return ;
}

int	Request::parseHeader(const Config &config)
{
	std::istringstream	stream(this->_raw_header);
	std::string			line;
	std::string			method_str;

	if (!std::getline(stream, line))
		return (_error = true, SERV_ERROR);
	std::istringstream	first_line(line);
	if (!(first_line >> method_str >> _request_target >> _version))
		return (_error = true, SERV_ERROR);
	_method = utils::strToMethod(method_str);
	_parseRequestTarget();
	if (!config.isAcceptedMethod(_method))
		return (_error = true, SERV_ERROR);
	while (std::getline(stream, line))
	{
		line = utils::stringTrim(line, "\r\n \t");
		if (line.empty())
			return (_error = true, SERV_ERROR);
		if (_parseHeaderLine(line, config) == SERV_ERROR)
			return (SERV_ERROR);
	}
	if (this->_accept == FTYPE_NONE)
		this->_accept = FTYPE_ANY;
	if (this->_query_cookies == NULL)
		this->_query_cookies = new Cookie();
	return (0);
}

std::string	&Request::getRawHeader()
{
	return (_raw_header);
}

std::string &Request::getRawBody()
{
	return (_raw_body);
}

const std::string &Request::getRawBody() const
{
	return (_raw_body);
}

Method	Request::getMethod() const
{
	return (_method);
}

std::string	Request::getRequestTarget() const
{
	return (_request_target);
}

std::string Request::getVersion() const
{
	return (_version);
}

size_t	Request::getContentLength() const
{
	return (_content_length);
}

bool	Request::isChunked() const
{
	return (_chunked);
}

std::map<std::string, std::string>	Request::getHeaders(void) const
{
	return (_headers);
}

bool	Request::getError() const
{
	return (_error);
}

const Cookie	&Request::getQueryCookies()
{
	if (!this->_query_cookies)
		this->_query_cookies = new Cookie();
	return (*this->_query_cookies);
}

ContentTypes	Request::getAccept() const
{
	return (this->_accept);
}

std::string	Request::getQueryString() const
{
	return (this->_query_string);
}

void	Request::setMethod(Method method)
{
	this->_method = method;
}

bool	Request::headerHasField(const std::string field)
{
	return (_headers.find(field) != _headers.end());
}

std::string	Request::headerGetField(const std::string field)
{
	if (this->_headers.find(field) != this->_headers.end())
		return (this->_headers[field]);
	return ("");
}

int		Request::_parseRequestTarget()
{
	if (_request_target.find("?") != std::string::npos)
	{
		_query_string = _request_target.substr(_request_target.find("?") + 1, _request_target.length());
		_request_target.erase(_request_target.find("?"), _request_target.length());
	}
	if (utils::startsWith(_request_target, "http://"))
		_request_target.erase(0, 8);
	if (utils::startsWith(_request_target, "www"))
		_request_target.erase(0, _request_target.find("/"));
	return (0);
}

int		Request::_parseHeaderLine(std::string line, const Config &config)
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
		}
		else if (key == "Accept")
			this->_accept = static_cast<ContentTypes>(this->_accept | utils::strToContentType(value));
		else if (headerHasField(key))
			this->_headers[key] = _headers[key] + "; " + value;
		else
			this->_headers[key] = value;
		if (key == "Transfer-Encoding" && utils::toLower(value).find("chunked") != std::string::npos)
			_chunked = true;
		else if (key == "Content-Length")
		{
			_content_length = std::atol(value.c_str());
			if (_content_length < 0 || _content_length > config.max_body_size)
				return (_error = true, SERV_ERROR);
		}
	}
	else
		return (_error = true, SERV_ERROR);
	return (0);
}

std::ostream	&operator<<(std::ostream &os, const Request &src)
{
	Method										method = src.getMethod();
	const std::map<std::string, std::string>	&headers = src.getHeaders();
	std::string									methodStr;

	if (src.getError())
	{
		os << "Parse error detected: " << src.getError() << std::endl;
		return (os);
	}
	methodStr = utils::methodToStr(method);
	os << "Method:\t\t" << methodStr << std::endl
		<< "Target:\t\t" << src.getRequestTarget() << std::endl
		<< "Version:\t" << src.getVersion() << std::endl
		<< "Headers:\t" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		os << "\t\t" << it->first << ": " << it->second << std::endl;
	os << "Body:\n" << src.getRawBody() << std::endl;
	os << "DONE" << std::endl;
	return (os);
}
