/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
<<<<<<< HEAD
/*   By: vviterbo <vviterbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 20:07:40 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/03 14:24:22 by vviterbo         ###   ########.fr       */
=======
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 20:07:40 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/02 17:07:53 by ego              ###   ########.fr       */
>>>>>>> CGI_check
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

/**
 * @brief Constructs a web server instance with its own ServerCore.
 *
 * @param config Server configuration.
 */
WebServ::WebServ(const ServerConfig *config)
{
	this->_config = config;
	std::cout << *this->_config << std::endl;
	this->_core = new ServerCore(config);
	this->_clients.resize(this->_config->max_clients);
	this->_processing_queue.resize(0);
}

/**
 * @brief Destructor freeing clients and core.
 */
WebServ::~WebServ()
{
	for (unsigned int i = 0; i < _config->max_clients; ++i)
	{
		if (this->_clients[i] != NULL)
		{
			delete this->_clients[i];
			this->_clients[i] = NULL;
		}
	}
	if (this->_core)
		delete this->_core;
	std::cerr << RED << "Destroying server" << RESET << std::endl;
}

/**
 * @brief Initializes sockets and poll entries.
 *
 * @return 0 on success, SERV_ERROR on failure.
 */
int	WebServ::Init()
{
	if (!this->_config || !this->_core)
		return (SERV_ERROR);
	if (_core->init() == SERV_ERROR)
		return (SERV_ERROR);
	this->_core->pollAdd(this->_core->getFd(), POLLIN, -1);
	std::cout << BOLD_GREEN << "[OK]" << GREEN << " WebServ initialized!" << RESET << std::endl;
	return (0);
}

/**
 * @brief Performs one loop iteration: updates queue then processes it.
 *
 * @return 0 on success, SERV_ERROR on fatal errors.
 */
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

/**
 * @brief Moves ready clients into the processing queue based on poll events.
 *
 * @return 0 on success, SERV_ERROR on fatal server error.
 */
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
				 std::cerr << RED << "[UpdateQueue] Server error: closing all connections !" << RESET << std::endl;
				return (SERV_ERROR);
			}
			else
			{
				std::cout << RED << "[UpdateQueue] Removing client " << event->client_id << " due to error" << RESET << std::endl;
				if (this->_clients[event->client_id])
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
				{
					//new_client->setState(TRY_ACCEPTING);
					//new_client->setTimeLimit(utils::getTime() + this->_config->timeout);
					this->_processing_queue.push_front(new_client);
				}
				else
					std::cerr << RED << "[UpdateQueue] Error creating Client instance" << RESET << std::endl;
			}
			else
			{
				if (this->_clients[event->client_id] && this->_clients[event->client_id]->getState() == DONE)
				{
					std::cout << MAGENTA << "[UpdateQueue] Client " << event->client_id << " added to processing queue" << RESET << std::endl;
					this->_clients[event->client_id]->setState(INIT);
					this->_clients[event->client_id]->setTimeLimit(utils::getTime() + this->_config->timeout);
					this->_processing_queue.push_back(this->_clients[event->client_id]);
				}
			}
		}
	}
	//utils::printProcessQueue(this->_processing_queue);
	//utils::printClients(this->_clients);
	return (0);
}

/**
 * @brief Handles one client from the processing queue.
 *
 * @return Error code from client handling.
 */    
int	WebServ::ProcessQueue()
{
	Client	*next_client;
	int		error;

	if (this->_processing_queue.empty())
		return (0);
	while (!this->_processing_queue.empty() && this->_processing_queue.front()->getTimeLimit() < utils::getTime())
	{
		std::cout << "Client " << this->_processing_queue.front()->getId() << " timed out" << std::endl;
		removeClient(this->_processing_queue.front()->getId());
		this->_processing_queue.pop_front();
	}
	next_client = this->_processing_queue.front();
	this->_processing_queue.pop_front();
	if (!next_client)
		return (ERR_NONE);
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

/**
 * @brief Creates a new Client or returns NULL if capacity reached.
 *
 * @return Pointer to new Client or NULL on failure/limit.
 */
Client	*WebServ::newClient()
{
	unsigned int	indx;

	for (indx = 0; indx < this->_config->max_clients; ++indx)
	{
		if (!this->_clients[indx])
			break;
	}
	if (indx == this->_config->max_clients)
	{
		std::cerr << RED << "[newClient] Cannot accept new clients, limit reached!" << RESET << std::endl;
		return (NULL);
	}
	this->_clients[indx] = new Client(this->_config, this->_core);
	this->_clients[indx]->setClientId(indx);
	this->_clients[indx]->setState(TRY_ACCEPTING);
	std::cout << GREEN << "[newClient] Client " << indx << " created." << RESET << std::endl;
	return (this->_clients[indx]);
}

/**
 * @brief Removes a client by index and cleans processing queue references.
 *
 * @param indx Client index.
 *
 * @return 0 on completion.
 */
int	WebServ::removeClient(size_t indx)
{
	size_t	i = 0;

	if (indx < this->_config->client_limit && this->_clients[indx] != NULL)
	{
		std::cout << RED << "[removeClient] Removing client " << indx << RESET << std::endl;
		std::deque<Client *>::iterator it = this->_processing_queue.begin();
		while (it != this->_processing_queue.end())
		{

			if (*it == this->_clients[indx])
			{
				this->_processing_queue.erase(it);
				it = this->_processing_queue.begin();
				std::advance(it, i);
			}
			else
			{
				++it;
				++i;
			}
		}
		delete this->_clients[indx];
		this->_clients[indx] = NULL;
	}
	return (0);
}
