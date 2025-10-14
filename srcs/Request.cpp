/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:12:49 by ego               #+#    #+#             */
/*   Updated: 2025/10/14 10:55:42 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(std::map<std::string, Cookie *> *all_cookies) : _all_cookies(all_cookies)
{
	this->_query_cookies.resize(0);
	this->_accept = FTYPE_NONE;
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
		_method = other._method;
		_requestTarget = other._requestTarget;
		_version = other._version;
		_rawBody = other._rawBody;
		_headers = other._headers;
		_error = other._error;
		_accept = other._accept;
	}
	return (*this);
}

Request::~Request(void) {}

int		Request::parseRequest(std::string request, Config *config)
{
	std::istringstream			stream(request);
	std::string					line;
	std::vector<std::string>	line_split;

	_error = false;
	if (!std::getline(stream, line))
	{
		std::cerr << "Empty request" << std::endl;
		_error = true;
		return (SERV_ERROR);
	}
	line_split = utils::stringSplit(line, "\\r\\n");
	if (line_split.size() == 0)
	{
		std::cerr << "Empty request" << std::endl;
		_error = true;
		return (SERV_ERROR);
	}
	std::istringstream	firstLine(line_split[0]);
	std::string			methodStr;

	if (!(firstLine >> methodStr >> _requestTarget >> _version))
	{
		_error = true;
		return (SERV_ERROR);
	}
	parseRequestTarget();
	_method = utils::strToMethod(methodStr);
	if (!config->isAcceptedMethod(_method))
	{
		_error = true;
	}
	Cookie::removeExpired(_all_cookies);
	for (unsigned int i = 1; i < line_split.size(); ++i)
	{
		if (utils::stringTrim(line, "\r\n \t").length() == 0)
			continue;
		parseHeaderLine(config, line_split[i]);
	}
	while (std::getline(stream, line) && line != "\r")
	{
		line_split = utils::stringSplit(line, "\\r\\n");
		for (unsigned int i = 1; i < line_split.size(); ++i)
		{
			if (utils::stringTrim(line, "\r\n \t").length() == 0)
				continue;
			parseHeaderLine(config, line_split[i]);
		}
	}
	if (_query_cookies.size() == 0)
		_query_cookies.push_back(Cookie::createSession(config, _all_cookies));
	if (_accept == FTYPE_NONE)
		_accept = FTYPE_ANY;

	size_t			expected_size = 0;
	size_t			bytes_read = 0;
	std::string		body_str = "";

	if (_headers.count("Content-Length"))
	{
		expected_size = std::atoi(_headers["Content-Length"].c_str());
		if (expected_size > config->max_body_size)
		{
			std::cerr << "Bad content length" << std::endl;
			_error = true;
			return (SERV_ERROR);
		}
	}
	else
		expected_size = config->max_body_size;
	std::vector<char> buffer(config->buffer_size);
	stream.read(&buffer[0], buffer.size());
	bytes_read = stream.gcount();
	while (stream && bytes_read && body_str.size() < expected_size)
	{
		body_str += std::string(buffer.begin(), buffer.end()).substr(0, bytes_read);
		stream.read(&buffer[0], buffer.size());
       	bytes_read = stream.gcount();
	}
	if (_rawBody.size() >= expected_size)
	{
		std::cerr << "Bad content length" << std::endl;
		_error = true;
		return (SERV_ERROR);
	}
	return (0);
}

int		Request::parseRequestTarget()
{
	if (_requestTarget.find("?") != std::string::npos)
	{
		_query_string = _requestTarget.substr(_requestTarget.find("?") + 1, _requestTarget.length());
		_requestTarget.erase(_requestTarget.find("?"), _requestTarget.length());
	}
	if (utils::startsWith(_requestTarget, "http://"))
		_requestTarget.erase(0, 8);
	if (utils::startsWith(_requestTarget, "www"))
		_requestTarget.erase(0, _requestTarget.find("/"));
	return (0);
}

int		Request::parseHeaderLine(Config *config, std::string line)
{
	std::vector<std::string>	field_split = utils::stringSplit(line, ": ");
	Cookie						*cookie;
	
	if (field_split.size() == 2)
	{
		std::string	key = field_split[0];
		std::string	value = field_split[1];
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);
		if (!value.empty() && value[value.size() - 1] == '\r')
			value.erase(value.size() - 1);
		if (key == "Cookie")
		{
			cookie = Cookie::getSession(config, this->_all_cookies, value);
			if (cookie && cookie->applyToPath(_requestTarget))
				this->_query_cookies.push_back(cookie);
		}
		else if (key == "Accept")
			this->_accept = static_cast<ContentTypes>(this->_accept | utils::strToContentType(value));
		else if (headerHasField(key))
			this->_headers[key] = _headers[key] + "; " + value;
		else
			this->_headers[key] = value;
	}
	else
	{
		std::cerr << "Invalid Header in Request" << std::endl;
		_error = true;
		return (SERV_ERROR);
	}
	return (0);
}

Method	Request::getMethod(void) const
{
	return (_method);
}

std::string	Request::getRequestTarget(void) const
{
	return (_requestTarget);
}

std::string Request::getVersion(void) const
{
	return (_version);
}

const std::string &Request::getRawBody(void) const
{
	return (_rawBody);
}

std::map<std::string, std::string>	Request::getHeaders(void) const
{
	return (_headers);
}

bool	Request::getError(void) const
{
	return (_error);
}
		

ContentTypes	Request::getAccept(void) const
{
	return (this->_accept);
}

void	Request::setMethod(Method method)
{
	this->_method = method;
}

bool	Request::headerHasField(const std::string field)
{
	return (_headers.find(field) != _headers.end());
}

std::string		Request::headerGetField(const std::string field)
{
	if (this->_headers.find(field) != this->_headers.end())
		return (this->_headers[field]);
	return ("");
}

std::vector<Cookie *>		Request::getQueryCookies()
{
	return (this->_query_cookies);
}

std::string					Request::getQueryString() const
{
	return (this->_query_string);
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
