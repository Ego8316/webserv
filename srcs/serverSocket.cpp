/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 13:40:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/28 19:15:28 by victorviter      ###   ########.fr       */
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

int	serverSocket::socketInit(Config &config)
{
	this->_domain = config.domain;
	this->_type = config.type;
	this->_protocol = config.protocol;
	this->_server_fd = socket(this->_domain, this->_type, this->_protocol);
	if (this->_server_fd == -1)
		std::cerr << "Socket init failed" << std::endl;
	else if (this->setSockOpt() == -1)
	{
		close(this->_server_fd);
		this->_server_fd = -1;
	}
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
	std::cout << "Binding to port number " << portNumber << " ok !" << std::endl;
	return (success);
}

int		serverSocket::socketAcceptClient(Client *new_client)
{
	new_client->setFd(accept(this->getFd(), (struct sockaddr*)&new_client->getClientAddr(),\
		&new_client->getClientLen()));
	if (new_client->getFd() == -1)
		std::cerr << "Accept failed: " << strerror(errno) << std::endl;
	return (new_client->getFd());
}

int		serverSocket::setSockOpt()
{
    int opt = 1;

    if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
        std::cerr << "Failed to set socket options: " << strerror(errno) << std::endl;
        return (-1);
	}
	return (0);
}

int		serverSocket::socketListen(Config config)
{
	if (listen(this->_server_fd, config.backlog) == -1) //TODO make this a serverSocket member function
	{
		std::cerr << "Socket listen failed" << std::endl;
		return (-1);
	}
	return (0);
}
