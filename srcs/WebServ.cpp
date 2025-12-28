/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 20:07:40 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/28 22:53:41 by victorviter      ###   ########.fr       */
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
	utils::logMsg(__PRETTY_FUNCTION__, WARN, "Shutting down server", -1);
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
	utils::logMsg(__PRETTY_FUNCTION__, WARN, "WebServ initialized!", -1, GREEN);
	return (0);
}

/**
 * @brief Performs one loop iteration: updates queue then processes it.
 *
 * @return 0 on success, SERV_ERROR on fatal errors.
 */
int	WebServ::Run()
{
	if (this->updateQueue() == SERV_ERROR)
	{
		utils::logMsg(__PRETTY_FUNCTION__, ERROR, "updateQueue returned an error", -1);
		return (SERV_ERROR);
	}
	if (this->_processQueue() == SERV_ERROR)
	{
		utils::logMsg(__PRETTY_FUNCTION__, ERROR, "_processQueue returned an error", -1);
		return (SERV_ERROR);
	}
	return (0);
}

/**
 * @brief Moves ready clients into the processing queue based on poll events.
 *
 * @return 0 on success, SERV_ERROR on fatal server error.
 */
int	WebServ::updateQueue()
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
				utils::logMsg(__PRETTY_FUNCTION__, ERROR, "Poll returned an error on the server socket: closing all connections", -1);
				return (SERV_ERROR);
			}
			else
			{
				utils::logMsg(__PRETTY_FUNCTION__, ERROR, "Removing client due to error", event->client_id);
				if (this->_clients[event->client_id])
					_removeClient(event->client_id);
			}
		}
		else
		{
			if (event->server == true)
			{
				utils::logMsg(__PRETTY_FUNCTION__, INFO, "New connection detected. Creating client instance", -1);
				Client	*new_client = _newClient();
				if (new_client)
				{
					this->_processing_queue.push_front(new_client);
				}
				else
					utils::logMsg(__PRETTY_FUNCTION__, ERROR, "Failed creating Client instance", -1);
			}
			else
			{
				if (this->_clients[event->client_id] && this->_clients[event->client_id]->getState() == DONE)
				{
					utils::logMsg(__PRETTY_FUNCTION__, INFO, "Added request to queue", event->client_id);
					this->_clients[event->client_id]->setState(INIT);
					this->_clients[event->client_id]->setTimeLimit(utils::getTime() + this->_config->timeout);
					this->_processing_queue.push_back(this->_clients[event->client_id]);
				}
			}
		}
	}
	return (0);
}

/**
 * @brief Handles one client from the processing queue.
 *
 * @return Error code from client handling.
 */    
int	WebServ::_processQueue()
{
	Client	*next_client;
	int		error;

	if (this->_processing_queue.empty())
		return (0);
	while (!this->_processing_queue.empty() && this->_processing_queue.front()->getTimeLimit() < utils::getTime())
	{
		sendTimeOut(this->_processing_queue.front());
		_removeClient(this->_processing_queue.front()->getId());
	}
	if (this->_processing_queue.empty())
		return (0);
	next_client = this->_processing_queue.front();
	this->_processing_queue.pop_front();
	if (!next_client)
		return (ERR_NONE);
	error = next_client->handleEvent();
	if (error == KILL_SERVER)
		return (SERV_ERROR);
	if (error == KILL_CLIENT)
		_removeClient(next_client->getId());
	else if (next_client->getState() != DONE && error <= WOULD_BLOCK)
		this->_processing_queue.push_back(next_client);
	else if (next_client->getState() == DONE)
		utils::logMsg(__PRETTY_FUNCTION__, INFO, "Finished processing request", next_client->getId());
	else
		utils::logMsg(__PRETTY_FUNCTION__, INFO, "Request failed after returning error in state " \
			+ utils::stateToStr(next_client->getState()) + " removed from queue", next_client->getId());
	return (error);
}

/**
 * @brief Creates a new Client or returns NULL if capacity reached.
 *
 * @return Pointer to new Client or NULL on failure/limit.
 */
Client	*WebServ::_newClient()
{
	unsigned int	indx;

	for (indx = 0; indx < this->_config->max_clients; ++indx)
	{
		if (!this->_clients[indx])
			break;
	}
	if (indx == this->_config->max_clients)
	{
		utils::logMsg(__PRETTY_FUNCTION__, ERROR, " Cannot accept new clients, limit reached!", -1);
		return (NULL);
	}
	this->_clients[indx] = new Client(this->_config, this->_core);
	this->_clients[indx]->setClientId(indx);
	this->_clients[indx]->setState(TRY_ACCEPTING);
	return (this->_clients[indx]);
}

/**
 * @brief Removes a client by index and cleans processing queue references.
 *
 * @param indx Client index.
 *
 * @return 0 on completion.
 */
int	WebServ::_removeClient(size_t indx)
{
	size_t	i = 0;

	if (indx < this->_config->max_clients && this->_clients[indx] != NULL)
	{
		utils::logMsg(__PRETTY_FUNCTION__, WARN, "Removing client", indx, GREEN);
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

void	WebServ::sendTimeOut(Client *client)
{
	utils::logMsg(__PRETTY_FUNCTION__, WARN, "Request timed out", client->getId());
	if (client->getFd() == -1)
		return ;
	if (client->getResponse() == NULL)
		client->setResponse(new Response(client->getId()));
	RequestHandler::handleError(client->getResponse(), HTTP_TIMEOUT, *this->_config);
	client->setState(SENDING_STRING);
	if (client->handleEvent() == ERR_NONE)
		utils::logMsg(__PRETTY_FUNCTION__, INFO, "TimeOut notification successfully sent", client->getId()); 
	else
		utils::logMsg(__PRETTY_FUNCTION__, INFO, "Could not send TimeOut notification", client->getId()); 
	return ;
}