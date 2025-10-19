/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 20:07:40 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/19 16:10:20 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ(Config *config)
{
	this->_config = config;
	std::cout << *this->_config << std::endl;
	this->_server = new serverSocket(this->_config);
	if (this->_server->getFd() < 0)
		return ;
	this->_poll = new serverPoll(this->_config);
	this->_clients.resize(this->_config->client_limit);
}

WebServ::WebServ(std::string config_file)
{
	this->_config = new Config(config_file);
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
	if (this->_server)
		delete this->_server;
	if (this->_poll)
		delete this->_poll;
}

int WebServ::WebServInit()
{
	if (!this->_config || !this->_server || !this->_poll)
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
	this->WebServUpdateQueue();
	this->WebServProcessQueue();
	return (0);
}

int WebServ::WebServUpdateQueue()
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
				this->_processing_queue.push_back(newClient());
			else
			{
				if (this->_clients[event->client_id]->getState() == DONE)
				{
					std::cout << "Client " << event->client_id << " added to processing queue" << std::endl;
					this->_clients[event->client_id]->setState(TRY_ACCEPTING);
					this->_processing_queue.push_back(this->_clients[event->client_id]);
				}
			}
		}
	}
	return (0);
}

int WebServ::WebServProcessQueue()
{
	Client	*next_client;

	if (this->_processing_queue.empty())
		return (0);
	next_client = this->_processing_queue.front();
	this->_processing_queue.pop_front();
	next_client->handleEvent();
	if (next_client->getState() != DONE)
		this->_processing_queue.push_back(next_client);
	return (0);
}

Client	 *WebServ::newClient()
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
		return (NULL);
	}
	this->_clients[indx] = new Client(this->_config, this->_poll);
	this->_clients[indx]->setClientId(indx);
	return (this->_clients[indx]);
}

int WebServ::removeClient(int indx)
{
	this->_poll->pollRemove(indx);
	
	if (this->_clients[indx] != NULL)
	{
		if (this->_clients[indx]->getState() != DONE)
		{
			std::deque<Client *>::iterator it = this->_processing_queue.begin();
			while (it != this->_processing_queue.end())
			{
				if (*it == this->_clients[indx])
				{
					this->_processing_queue.erase(it);
					break ;
				}
				++it;
			}
		}
		delete this->_clients[indx];
	}
	this->_clients[indx] = NULL;
	return (0);
}

int WebServ::WebServReboot()
{
	//TODO
	std::cerr << "Gné gné gné ca marche pas" << std::endl;
	return (SERV_ERROR);
}
