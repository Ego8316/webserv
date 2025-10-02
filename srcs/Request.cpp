/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:12:49 by ego               #+#    #+#             */
/*   Updated: 2025/10/02 17:53:35 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() {}

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
	}
	return (*this);
}

Request::~Request(void) {}

int		Request::parseRequest(std::string request)
{
	std::istringstream			stream(request);
	std::string					line;
	std::vector<std::string>	line_split;

	_error = NONE;
	if (!std::getline(stream, line))
	{
		std::cerr << "Empty request" << std::endl;
		_error = INVALID_REQUEST_LINE;
		return (SERV_ERROR);
	}
	line_split = stringSplit(line, "\\r\\n");
	if (line_split.size() == 0)
	{
		std::cerr << "Empty request" << std::endl;
		_error = INVALID_REQUEST_LINE;
		return (SERV_ERROR);
	}
	std::istringstream	firstLine(line_split[0]);
	std::string			methodStr;

	if (!(firstLine >> methodStr >> _requestTarget >> _version))
	{
		_error = INVALID_REQUEST_LINE;
		return (SERV_ERROR);
	}
	if (methodStr == "GET") _method = GET;
	else if (methodStr == "POST") _method = POST;
	else _method = DELETE;
	for (unsigned int i = 1; i < line_split.size(); ++i)
	{
		if (line == "\\r\\n")
			continue;
		this->parseHeaderLine(line_split[i]);
	}
	while (std::getline(stream, line) && line != "\r")
	{
		line_split = stringSplit(line, "\\r\\n");
		for (unsigned int i = 1; i < line_split.size(); ++i)
		{
			if (line == "\\r\\n")
				continue;
			this->parseHeaderLine(line_split[i]);
		}
	}
	std::ostringstream	bodyStream;
	bodyStream << stream.rdbuf();
	_rawBody = bodyStream.str();
	if (_headers.count("Content-Length"))
	{
		size_t	expected = std::atoi(_headers["Content-Length"].c_str());
		if (_rawBody.size() != expected)
		{
			std::cerr << "Bad content length" << std::endl;
			_error = BAD_CONTENT_LENGTH;
			return (SERV_ERROR);
		}
	}
	return (0);
}

int		Request::parseHeaderLine(std::string line)
{
	std::vector<std::string>	field_split = stringSplit(line, ": ");

	if (field_split.size() == 2)
	{
		std::string	key = field_split[0];
		std::string	value = field_split[1];
		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);
		if (!value.empty() && value[value.size() - 1] == '\r')
			value.erase(value.size() - 1);
		if (headerHasField(key))
			this->_headers[key] = _headers[key] + "; " + value;
		else
			this->_headers[key] = value;
			
	}
	else
	{
		std::cerr << "Invalid Header in Request" << std::endl;
		_error = INVALID_HEADER;
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

std::string Request::getRawBody(void) const
{
	return (_rawBody);
}

std::map<std::string, std::string>	Request::getHeaders(void) const
{
	return (_headers);
}

int	Request::getError(void) const
{
	return (_error);
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

std::ostream	&operator<<(std::ostream &os, const Request &src)
{
	Method										method = src.getMethod();
	const std::map<std::string, std::string>	&headers = src.getHeaders();
	std::string									methodStr;

	if (src.getError() != NONE)
	{
		os << "Parse error detected: " << src.getError() << std::endl;
		return (os);
	}
	if (method == GET)
		methodStr = "GET";
	else if (method == POST)
		methodStr = "POST";
	else
		methodStr = "DELETE";

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
