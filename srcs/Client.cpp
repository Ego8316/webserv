/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/28 19:35:20 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
{
	this->_client_len = sizeof(this->_client_addr);
}

Client::Client(const Client &other) : _client_addr(other._client_addr), _client_len(other._client_len) {}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		this->_client_addr = other._client_addr;
		this->_client_len = other._client_len;
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
		
int		Client::socketRead(char *buffer, int &bytes_read) //TODO
{
	if (recv(this->_client_fd, buffer, bytes_read, 0) == SERV_ERROR)
	{
		std::cerr << "Receive failed\n";
		return (SERV_ERROR);
	}
	return (0);
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
	/* For now it just echoes back the message for testing purposes */
	char 	    buffer[10];
	ssize_t     bytes_read;

	std::cout << "handling event" << std::endl;
	while ((bytes_read = recv(this->_client_fd, buffer, sizeof(buffer), 0)) > 0)
	{
		std::cout << "Reading input" << std::endl;
		if (std::strncmp(buffer, "close", 5) == 0)
		{
			std::cout << "Closing connection ..." << std::endl;
			return (SERV_ERROR);
		}
		else if (send(this->_client_fd, buffer, bytes_read, 0) == SERV_ERROR)
		{
			std::cerr << "Send failed\n";
			return (SERV_ERROR);
		}
			return (0);
	}
	return (0);
}

void	Client::setClientId(int id)
{
	this->_client_id = id;
}