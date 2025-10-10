/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 20:07:40 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/10 18:35:24 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ(Config *config)
{
	this->_config = config;
	std::cout << *this->_config << std::endl;
	Cookie(this->_config);
	this->_cookie_sessions = new std::map<std::string, Cookie *>;
	this->_server = new serverSocket(this->_config);
	if (this->_server->getFd() < 0)
		return ;
	this->_poll = new serverPoll(this->_config);
	this->_clients.resize(this->_config->client_limit);
}

WebServ::WebServ(std::string config_file)
{
	this->_config = new Config(config_file);
	Cookie(this->_config);
	this->_cookie_sessions = new std::map<std::string, Cookie *>;
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

WebServ::~WebServ()
{
	for (unsigned int i = 0; i < _clients.size(); ++i)
	{
		if (this->_clients[i] != NULL)
			delete this->_clients[i];
	}
	for (std::map<std::string, Cookie *>::iterator it = (*this->_cookie_sessions).begin(); it != (*this->_cookie_sessions).end(); ++it)
	{
		if (it->second != NULL)
			delete it->second;
	}
	if (this->_cookie_sessions)
		delete this->_cookie_sessions;
	if (this->_server)
		delete this->_server;
	if (this->_poll)
		delete this->_poll;
}

int WebServ::WebServInit()
{
	if (!this->_config || !this->_cookie_sessions || !this->_server || !this->_poll)
		return (SERV_ERROR);
	this->_poll->pollAdd(this->_server->getFd(), POLLIN, -1);
	std::cout << "pollAdd \t ok !" << std::endl;
	if (this->_server->socketBind() == SERV_ERROR)
		return (SERV_ERROR);
	std::cout << "Socket Bind \t ok !" << std::endl;
	if (this->_server->socketListen() == SERV_ERROR)
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
		if (event->error)
		{
			if (event->server == true)
			{
				std::cerr << "Server Error closed connection" << std::endl;
				// TODO do a clean exit, probably will see that at the end when we know what need to be closed/cleaned
				return (SERV_ERROR);
			}
			else
			{
				std::cout << "removing client " << event->client_id << std::endl;
				removeClient(event->client_id);
			}
		}
		else
		{
			if (event->server == true)
			{
				if (this->newClient() == SERV_ERROR)
					std::cerr << "Failed to accept new client" << std::endl;
			}
			else
			{
				std::cout << "Client " << event->client_id << " handles event" << std::endl;
				this->_clients[event->client_id]->handleEvent();
			}
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
	this->_clients[indx] = new Client(this->_config, this->_cookie_sessions);
	if (this->_server->socketAcceptClient(this->_clients[indx]) == SERV_ERROR)
	{
		std::cerr << "Failed to accept new client" << std::endl;
		return (SERV_ERROR);
	}
	this->_clients[indx]->setClientId(indx);
	this->_poll->pollAdd(this->_clients[indx]->getFd(), POLLIN, indx);
	std::cout << "Accepted client " << indx << std::endl;
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
