/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 20:07:40 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/23 15:15:27 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ(const Config *config)
{
	this->_config = config;
	std::cout << *this->_config << std::endl;
	this->_core = new ServerCore(config);
	this->_clients.resize(this->_config->client_limit);
	this->_processing_queue.clear();
}

WebServ::WebServ(std::string config_file)
{
	this->_config = new Config(config_file);
	this->_core = new ServerCore(this->_config);
	this->_clients.resize(this->_config->client_limit);
	this->_processing_queue.clear();
}

WebServ::WebServ(const WebServ &other) : _config(other._config), _core(other._core), _clients(other._clients) {}

WebServ	&WebServ::operator=(const WebServ &other)
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

int	WebServ::Init()
{
	if (!this->_config || !this->_core)
		return (SERV_ERROR);
	if (_core->init() == SERV_ERROR)
		return (SERV_ERROR);
	this->_core->pollAdd(this->_core->getFd(), POLLIN, -1);
	std::cout << BOLD_GREEN << "[OK] WebServ initialized!" << RESET << std::endl;
	return (0);
}

int	WebServ::Run()
{
	if (this->UpdateQueue() == SERV_ERROR)
	{
		std::cout << RED << "[UpdateQueue] UpdateQueue returned an error" << RESET << std::endl;
		return (SERV_ERROR);
	}
	if (this->ProcessQueue() == SERV_ERROR)
	{
		std::cout << RED << "[UpdateQueue] ProcessQueue returned an error" << RESET << std::endl;
		return (SERV_ERROR);
	}
	return (0);
}

int	WebServ::UpdateQueue()
{
	std::vector<PollRevent>	events;

	events = this->_core->pollWatchRevent();
	if (events.size() == 0)
		return (0);
	for (std::vector<PollRevent>::iterator event = events.begin(); event != events.end(); ++event)
	{
		if (event->error)
		{
			if (event->server == true)
			{
				 std::cerr << RED << "[UpdateQueue] Server error: closing connection!" << RESET << std::endl;
				// TODO do a clean exit, probably will see that at the end when we know what need to be closed/cleaned
				return (SERV_ERROR);
			}
			else
			{
				std::cout << RED << "[UpdateQueue] Removing client " << event->client_id << " due to error" << RESET << std::endl;
				removeClient(event->client_id);
			}
		}
		else
		{
			if (event->server == true)
			{
				std::cout << GREEN << "[UpdateQueue] New connection detected. Creating client..." << RESET << std::endl;
				Client	*new_client = newClient();
				if (new_client)
					this->_processing_queue.push_back(new_client);
				else
					std::cerr << RED << "[UpdateQueue] Error creating Client instance" << RESET << std::endl;
			}
			else
			{
				if (this->_clients[event->client_id] && this->_clients[event->client_id]->getState() == DONE)
				{
					std::cout << MAGENTA << "[UpdateQueue] Client " << event->client_id << " added to processing queue" << RESET << std::endl;
					this->_clients[event->client_id]->setState(INIT);
					this->_processing_queue.push_back(this->_clients[event->client_id]);
				}
			}
		}
	}
	return (0);
}

int	WebServ::ProcessQueue()
{
	Client	*next_client;
	int		error;

	if (this->_processing_queue.empty())
		return (0);
	next_client = this->_processing_queue.front();
	this->_processing_queue.pop_front();
	error = next_client->handleEvent();
	if (error == KILL_SERVER)
		return (SERV_ERROR);
	if (error == KILL_CLIENT)
		removeClient(next_client->getId());
	else if (next_client->getState() != DONE && error <= WOULD_BLOCK)
		this->_processing_queue.push_back(next_client);
	else if (next_client->getState() == DONE)
		std::cout << GREEN << "[ProcessQueue] Client " << next_client->getId() << " finished processing." << RESET << std::endl;
	else
		std::cout << RED << "[ProcessQueue] Client " << next_client->getId() << 
			" has been removed from queue : returned error " << error << 
			" and in state " << next_client->getState() << RESET << std::endl;
	return (error);
}

Client	*WebServ::newClient()
{
	int indx;

	for (indx = 0; indx < this->_config->client_limit; ++indx)
	{
		if (!this->_clients[indx])
			break;
	}
	if (indx == this->_config->client_limit)
	{
		std::cerr << RED << "[newClient] Cannot accept new clients, limit reached!" << RESET << std::endl;
		return (NULL);
	}
	this->_clients[indx] = new Client(this->_config, this->_core);
	this->_clients[indx]->setClientId(indx);
	std::cout << GREEN << "[newClient] Client " << indx << " created." << RESET << std::endl;
	return (this->_clients[indx]);
}

int	WebServ::removeClient(int indx)
{
	this->_core->pollRemove(indx);
	if (this->_clients[indx] != NULL)
	{
		 std::cout << RED << "[removeClient] Removing client " << indx << RESET << std::endl;
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
