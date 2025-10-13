/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/13 12:31:17 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(Config *config, std::map<std::string, Cookie *> *all_cookies, serverPoll *poll)
	:	_all_cookies(all_cookies),
		_config(config),
		_poll(poll)
{
	this->_client_len = sizeof(this->_client_addr);
}

Client::Client(const Client &other) : _client_addr(other._client_addr), _client_len(other._client_len), _config(other._config) {}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		this->_client_addr = other._client_addr;
		this->_client_len = other._client_len;
		this->_config = other._config;
	}
	return (*this);
}

Client::~Client()
{
	if (this->_client_fd)
		close(this->_client_fd);
}

struct sockaddr_in	&Client::getClientAddr()
{
	return (this->_client_addr);
}

socklen_t   &Client::getClientLen()
{
	return (this->_client_len);
}

int     Client::getFd()
{
	return (this->_client_fd);
}

void    Client::setFd(int fd)
{
	this->_client_fd = fd;
}
		
int		Client::socketRead(char *buffer, int bytes_read) //TODO
{
	if (!this->_poll->pollAvailFor(this->_client_id, POLLIN))
		return (0);
	bytes_read = recv(this->_client_fd, buffer, bytes_read, 0);
	if (bytes_read == SERV_ERROR)
	{
		std::cerr << "Receive failed\n";
		return (SERV_ERROR);
	}
	std::memset(buffer + bytes_read, 0, this->_config->buffer_size - bytes_read);
	return (bytes_read);
}

int		Client::socketWrite(const char *buffer, int bytes_write) //TODO 
{
	if (!this->_poll->pollAvailFor(this->_client_id, POLLOUT))
		return (0);
	if (send(this->_client_fd, buffer, bytes_write, 0) == SERV_ERROR)
	{
		std::cerr << "Send failed\n";
		return (SERV_ERROR);
	}
	return (0);
}
		
int		Client::handleEvent()
{
	int			bytes_read;
	std::string	request_str;
	std::string	response_str;
	
	char	*buffer = new char[_config->buffer_size]; //TODO std::vector<char> buffer(1024);
	bytes_read = _config->buffer_size;
	while (bytes_read == _config->buffer_size)
	{
		bytes_read = socketRead(buffer, bytes_read);
		if (bytes_read == SERV_ERROR)
			return (SERV_ERROR);
		request_str += std::string(buffer).substr(0, bytes_read);
	}
	std::cout << "REQUEST = " << std::endl;
	std::cout << request_str << std::endl;
	if (request_str.length() == 0)
	{
		std::cerr << "Empty request. Ignoring..." << std::endl;
		return (SERV_ERROR);
	}
	Request	request(_all_cookies);
	request.parseRequest(request_str, *_config);
	std::cout << request << std::endl;
	std::vector<Cookie *>	cookies = request.getQueryCookies();
	Response	response = RequestHandler::handle(request, *_config, cookies);
	response_str = response.toString();
	std::cout << "RESPONSE =" << std::endl;
	std::cout << response_str << std::endl;
	if (socketWrite(response_str.c_str(), response_str.length()) == SERV_ERROR)
		return (SERV_ERROR);
	delete[] buffer;
	return (0); //TODO return err code
}

void	Client::setClientId(int id)
{
	this->_client_id = id;
}