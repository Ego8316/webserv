/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/08 15:01:29 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(Config *config, Cookie *cookies) : _config(config)
{
	this->_cookies = cookies;
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
	if (send(this->_client_fd, buffer, bytes_write, 0) == SERV_ERROR)
	{
		std::cerr << "Receive failed\n";
		return (SERV_ERROR);
	}
	return (0);
}
		
int		Client::handleEvent()
{
	Query	query(this->_config, this, this->_cookies);

	std::cout << "Client handling event" << std::endl;
	query.queryRespond();
	return (0); //TODO return err code
}

void	Client::setClientId(int id)
{
	this->_client_id = id;
}