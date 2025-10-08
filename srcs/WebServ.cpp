/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 20:07:40 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/08 16:19:56 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ(Config *config)
{
	this->_config = config;
	if (this->_config->getParseError() != NONE)
		return ;
	std::cout << *this->_config << std::endl;
	this->_cookies = new Cookie(this->_config);
	this->_server = new serverSocket(this->_config);
	if (this->_server->getFd() < 0)
		return ;
	this->_poll = new serverPoll(this->_config);
	this->_clients.resize(this->_config->client_limit);
}

WebServ::WebServ(std::string config_file)
{
	this->_config = new Config(config_file);
	if (this->_config->getParseError() != NONE)
		return ;
	this->_cookies = new Cookie(this->_config);
	this->_server = new serverSocket(this->_config);
	if (this->_server->getFd() < 0)
		return ;
	this->_poll = new serverPoll(this->_config);
	this->_clients.resize(this->_config->client_limit);
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

int WebServ::WebServInit()
{
	if (!this->_config || !this->_cookies || !this->_server || !this->_poll)
		return (SERV_ERROR);
	this->_poll->pollAdd(this->_server->getFd(), POLLIN, 0);
	std::cout << "pollAdd \t ok !" << std::endl;
	if (this->_server->socketBind() == -1)
		return (SERV_ERROR);
	std::cout << "Socket Bind \t ok !" << std::endl;
	if (this->_server->socketListen() == -1)
		return (SERV_ERROR);
	std::cout << "Socket Listen \t ok !" << std::endl;
	std::cout << "WebServ Init \t ok !" << std::endl;
	return (0);
}

int WebServ::WebServRun()
{
	std::vector<pollRevent>	events;

	events = this->_poll->pollWatchRevent();
	if (events.size() == 0)
		return (0);
	for (std::vector<pollRevent>::iterator event = events.begin(); event != events.end(); ++event)
	{
		if (event->is_error)
		{
			if (event->client_id == 0)
			{
				std::cerr << "poll Wait failed" << std::endl;
				// TODO do a clean exit, probably will see that at the end when we know what need to be closes/cleaned
				return (-1);	
			}
			else
				removeClient(event->client_id);
		}
		else
		{
			if (event->client_id == 0)
			{
				if (this->newClient() == -1)
					std::cerr << "Failed to accept new client" << std::endl;
			}
			else
				this->_clients[event->client_id - 1]->handleEvent();
		}
	}
	return (0);
}

int WebServ::newClient()
{
	int indx;

	for (indx = 0; indx < this->_config->client_limit; ++indx)
	{
		if (!this->_clients[indx])
			break;
	}
	if (indx == this->_config->client_limit)
	{
		std::cerr << "Cannot accept new clients" << std::endl;
		return (SERV_ERROR);
	}
	this->_clients[indx] = new Client(this->_config, this->_cookies);
	if (this->_server->socketAcceptClient(this->_clients[indx]) == -1)
	{
		std::cerr << "Failed to accept new client" << std::endl;
		return (SERV_ERROR);
	}
	this->_clients[indx]->setClientId(indx);
	this->_poll->pollAdd(this->_clients[indx]->getFd(), POLLIN, indx + 1);
	return (0);
}

int WebServ::removeClient(int indx)
{
	this->_poll->pollRemove(indx);
	if (this->_clients[indx] != NULL)
		delete this->_clients[indx];
	this->_clients[indx] = NULL;
	return (0);
}

int WebServ::WebServReboot()
{
	std::cerr << "Gné gné gné ca marche pas" << std::endl;
	return (SERV_ERROR);
}