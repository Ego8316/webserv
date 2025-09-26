/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 13:40:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/25 22:39:44 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverSocket.hpp"

serverSocket::serverSocket() {}

serverSocket::serverSocket(const serverSocket &other)
{
	this->_server_fd = other._server_fd;
	this->_domain = other._domain;
	this->_type = other._type;
	this->_protocol = other._protocol;
	this->_server_addr = other._server_addr;
}

serverSocket &serverSocket::operator=(const serverSocket &other)
{
	this->_server_fd = other._server_fd;
	this->_domain = other._domain;
	this->_type = other._type;
	this->_protocol = other._protocol;
	this->_server_addr = other._server_addr;
	return (*this);
}

serverSocket::~serverSocket()
{
	if (this->_server_fd > 0)
		close(this->_server_fd);
}

int	serverSocket::getFd()
{
	return (this->_server_fd);
}

int	serverSocket::socketInit(int domain, int type, int protocol)
{
	this->_server_fd = socket(domain, type, protocol);
	this->_domain = domain;
	this->_type = type;
	this->_protocol = protocol;
	if (this->_server_fd == -1)
		std::cerr << "Socket init failed" << std::endl;
	return (this->_server_fd);
}

int		serverSocket::socketBind(int portNumber)
{
	int	success;

	std::memset(&this->_server_addr, 0, sizeof(this->_server_addr));
	this->_server_addr.sin_family = this->_domain;
	this->_server_addr.sin_addr.s_addr = INADDR_ANY; //Bind to all available interfaces
	this->_server_addr.sin_port = htons(portNumber); //port # above 1024 are not priviledged
	success = ::bind(this->_server_fd, (struct sockaddr*)&this->_server_addr, sizeof(this->_server_addr));
	if (success == -1)
		std::cerr << "Socket bind failed: " << strerror(errno) << std::endl;
	return (success);
}

clientSocket	*serverSocket::socketAcceptClient()
{
	clientSocket *new_client = new clientSocket();

	std::cout << "Waiting on new client" << std::endl;
	new_client->setFd(accept(this->getFd(), (struct sockaddr*)&new_client->getClientAddr(),\
		&new_client->getClientLen()));
	if (new_client->getFd() == -1)
		std::cerr << "Accept failed: " << strerror(errno) << std::endl;
	return (new_client);
}
