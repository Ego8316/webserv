/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 20:07:40 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/19 17:11:52 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ(Config *config)
{
	this->_config = config;
	std::cout << *this->_config << std::endl;
	this->_core = new ServerCore(config);
	this->_clients.resize(this->_config->client_limit);
}

WebServ::WebServ(std::string config_file)
{
	this->_config = new Config(config_file);
	this->_core = new ServerCore(this->_config);
	this->_clients.resize(this->_config->client_limit);
}

WebServ::WebServ(const WebServ &other) : _config(other._config), _core(other._core), _clients(other._clients) {}

WebServ &WebServ::operator=(const WebServ &other)
{
	if (this != &other)
	{
		this->_config = other._config;
		this->_core = other._core;
		this->_core = other._core;
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
	if (this->_core)
		delete this->_core;
}

int WebServ::WebServInit()
{
	if (!this->_config || !this->_core)
		return (SERV_ERROR);
	if (_core->init() == SERV_ERROR)
		return (SERV_ERROR);
	this->_core->pollAdd(this->_core->getFd(), POLLIN, -1);
	std::cout << BOLD_GREEN << "[OK] WebServ initialized!" << RESET << std::endl;
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

	events = this->_core->pollWatchRevent();
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
					this->_clients[event->client_id]->setState(INPUT_READING);
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
	this->_clients[indx] = new Client(this->_config, this->_core);
	this->_clients[indx]->setClientId(indx);
	return (this->_clients[indx]);
}

int WebServ::removeClient(int indx)
{
	this->_core->pollRemove(indx);
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
