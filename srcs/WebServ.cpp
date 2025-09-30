/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 20:07:40 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 15:13:46 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ()
{
	this->_clients.resize(CLIENT_LIMIT);
}

WebServ::WebServ(const WebServ &other) : _config(other._config), _server(other._server), _poll(other._poll), _clients(other._clients) {}

WebServ &WebServ::operator=(const WebServ &other)
{
	if (this != &other)
	{
		this->_config = other._config;
		this->_server = other._server;
		this->_poll = other._poll;
		this->_clients = other._clients;
	}
	return (*this);
}

WebServ::~WebServ() {}

int		WebServ::WebServInit(std::string config_file)
{
	this->_config = Config(config_file);
	if (this->_config.getParseError() != NONE)
		return (SERV_ERROR);
	std::cout << "Config ok !" << std::endl;
	if (this->_server.socketInit(this->_config) == -1)
		return (SERV_ERROR);
	std::cout << "Socket Init ok !" << std::endl;
	this->_poll.pollAdd(this->_server.getFd(), POLLIN, 0);
	std::cout << "pollAdd ok !" << std::endl;
	if (this->_server.socketBind(this->_config.port_number) == -1)
		return (SERV_ERROR);
	std::cout << "Socket Bind ok !" << std::endl;
	if (this->_server.socketListen(this->_config) == -1)
		return (SERV_ERROR);
	std::cout << "Socket Listen ok !" << std::endl;
	std::cout << "WebServ Init OK" << std::endl;
	return (0);
}


int		WebServ::WebServRun()
{
	int	event;

	while (true)
	{
		event = this->_poll.pollWatchRevent(this->_config);
		std::cout << "Detected new event " << event << std::endl;
		if (event == -1)
		{
			std::cerr << "poll Wait failed" << std::endl;
			//TODO do a clean exit, probably will see that at the end when we know what need to be closes/cleaned
			return (-1);
		}
		else if (event == 0)
		{
			if (this->newClient() == -1)
			{
				std::cerr << "Failed to accept new client" << std::endl;
				return (-1);
			}
		}
		else if (event < 0)
			this->removeClient(CLIENT_ERR_IDX(event) - 1);
		else
			this->_clients[event - 1]->handleEvent();
	}
	return (0);
}

int		WebServ::newClient()
{
	int		indx;

	for (indx = 0; indx < this->_config.client_limit; ++indx)
	{
		if (!this->_clients[indx])
			break;
	}
	this->_clients[indx] = new Client(&this->_config);
	if (this->_server.socketAcceptClient(this->_clients[indx]) == -1)
	{
		std::cerr << "Failed to accept new client" << std::endl;
		return (SERV_ERROR);
	}
	this->_clients[indx]->setClientId(indx);
	this->_poll.pollAdd(this->_clients[indx]->getFd(), POLLIN, indx + 1);
	return (0);
}

int		WebServ::removeClient(int indx)
{
	this->_poll.pollRemove(indx);
	if (this->_clients[indx] != NULL)
		delete this->_clients[indx];
	this->_clients[indx] = NULL;
	return (0);
}
