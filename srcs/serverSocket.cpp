/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 13:40:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/02 13:33:25 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverSocket.hpp"

serverSocket::serverSocket(Config *config)
{
	this->_config = config;
	std::cout << this->_config->domain << " " << this->_config->type << " " << this->_config->protocol << std::endl;
	this->_server_fd = socket(this->_config->domain, this->_config->type, this->_config->protocol);
	if (this->_server_fd == -1)
		std::cerr << "Socket init failed" << std::endl;
	else if (this->setSockOpt() == -1)
	{
		std::cerr << "Socket setSockOpt failed" << std::endl;
		close(this->_server_fd);
		this->_server_fd = -1;
	}
	else
		std::cout << "server fd is now " << this->_server_fd << std::endl;
}

serverSocket::serverSocket(const serverSocket &other)
{
	this->_server_fd = other._server_fd;
	this->_server_addr = other._server_addr;
}

serverSocket &serverSocket::operator=(const serverSocket &other)
{
	this->_server_fd = other._server_fd;
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

int		serverSocket::socketBind()
{
	int	success;

	std::memset(&this->_server_addr, 0, sizeof(this->_server_addr));
	this->_server_addr.sin_family = this->_config->domain;
	this->_server_addr.sin_addr.s_addr = INADDR_ANY; //Bind to all available interfaces
	this->_server_addr.sin_port = htons(this->_config->port_number); //port # above 1024 are not priviledged
	success = ::bind(this->_server_fd, (struct sockaddr*)&this->_server_addr, sizeof(this->_server_addr));
	if (success == -1)
		std::cerr << "Socket bind failed: " << strerror(errno) << std::endl;
	std::cout << "Binding to port number " << this->_config->port_number << " ok !" << std::endl;
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

int		serverSocket::socketListen()
{
	if (listen(this->_server_fd, this->_config->incomming_queue_backlog) == -1)
	{
		std::cerr << "Socket listen failed" << std::endl;
		return (-1);
	}
	return (0);
}
