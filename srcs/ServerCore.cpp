/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCore.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 15:53:20 by ego               #+#    #+#             */
/*   Updated: 2025/11/29 14:47:31 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCore.hpp"

ServerCore::ServerCore(const Config *config)
{
	_config = config;
	_server_fd = SERV_ERROR;
	std::memset(&_server_addr, 0, sizeof(_server_addr));
	_poll_fds.resize(3 * _config->client_limit + 1); //3 per clients (1 for web commmunications, 2 for cgi pipes) + 1 for server
	return ;
}

ServerCore::ServerCore(const ServerCore &other)
{
	_config = other._config;
	_server_fd = other._server_fd;
	_server_addr = other._server_addr;
	_poll_fds = other._poll_fds;
	return ;
}

ServerCore	&ServerCore::operator=(const ServerCore &other)
{
	if (this != &other)
	{
		_config = other._config;
		_server_fd = other._server_fd;
		_server_addr = other._server_addr;
		_poll_fds = other._poll_fds;
	}
	return (*this);
}

ServerCore::~ServerCore()
{
	if (_server_fd > 0)
	{
		close(this->_server_fd);
		this->_server_fd = -1;
	}
	return ;
}

int	ServerCore::getFd() const
{
	return (this->_server_fd);
}

std::vector<struct pollfd>	&ServerCore::getPollFds()
{
	return (this->_poll_fds);
}

int	ServerCore::init()
{
	if (!_socketCreate())
	{
		std::cerr << BOLD_RED << "[KO] Socket creation failed: " << RED << strerror(errno) << RESET << std::endl;
		return (SERV_ERROR);
	}
	std::cout << BOLD_GREEN << "[OK]" << GREEN << " Socket successfully created" << RESET << std::endl;
	if (!_socketSetOptions())
	{
		std::cerr << BOLD_RED << "[KO] Socket configuration failed: " << RED <<  strerror(errno) << RESET << std::endl;
		return (SERV_ERROR);
	}
	std::cout << BOLD_GREEN << "[OK]" << GREEN << " Socket successfully configured" << RESET << std::endl;
	if (!_socketBind())
	{
		std::cerr << BOLD_RED << "[KO] Socket bind failed: " << RED << strerror(errno) << RESET << std::endl;
		return (SERV_ERROR);
	}
	std::cout << BOLD_GREEN << "[OK]" << GREEN << " Socket successfully binded" << RESET << std::endl;
	if (!_socketListen())
	{
		std::cerr << BOLD_RED << "[KO] Socket listen failed: " << RED << strerror(errno) << RESET << std::endl;
		return (false);
	}
	std::cout << BOLD_GREEN << "[OK]" << GREEN << " Socket successfully listened" << RESET << std::endl;
	return (0);
}

int	ServerCore::socketAcceptClient(Client *new_client)
{
	new_client->setFd(accept(_server_fd,
		(struct sockaddr *)&new_client->getClientAddr(),
		&new_client->getClientLen()));
	if (new_client->getFd() == SERV_ERROR)
		std::cerr << BOLD_RED << "Accept failed: " << RED << strerror(errno) << RESET << std::endl;
	else if (std::string(OS_NAME) == "macOs")
		setNonBlocking(new_client->getFd());
	return (new_client->getFd());
}

int	ServerCore::socketRead(char *buffer, int bytes_read, Client *client)
{
	if (!pollAvailFor(client->getId(), POLLIN))
		return (WBLOCK);
	int	bytes_received = recv(client->getFd(), buffer, bytes_read, MSG_DONTWAIT);
	if (bytes_received >= 0)
		std::memset(buffer + bytes_received, 0, this->_config->buffer_size - bytes_received);
	return (bytes_received);
}

int	ServerCore::socketWrite(const char *buffer, int bytes_write, Client *client)
{
	if (!pollAvailFor(client->getId(), POLLOUT))
		return (WBLOCK);
	int	bytes_sent = send(client->getFd(), buffer, bytes_write, MSG_DONTWAIT);
	return (bytes_sent);
}

void	ServerCore::pollAdd(int fd, nfds_t event, int idx)
{
	if (idx == IS_SERVER)
		idx = _poll_fds.size() - 1;
	struct pollfd	new_poll_fd;
	new_poll_fd.fd = fd;
	new_poll_fd.events = event;
	_poll_fds[idx] = new_poll_fd;
	return ;
}

void	ServerCore::pollRemove(int idx)
{
	std::memset(&_poll_fds[idx], 0, sizeof(pollfd));
}

std::vector<PollRevent>	ServerCore::pollWatchRevent()
{
	std::vector<PollRevent>	ret;
	PollRevent				revent;
	int						num_event;

	ret.resize(0);
	num_event = _pollWait();
	if (num_event < 0)
		return (ret);
	for (unsigned int i = 0; i < static_cast<unsigned int>(_config->client_limit) + 1; ++i)
	{
		if (i == static_cast<unsigned int>(_config->client_limit))
			i = this->_poll_fds.size() - 1;
		revent.error = false;
		revent.server = false;
		revent.client_id = -1;
		revent.revent = 0;
		if (_poll_fds[i].fd == 0)
			continue ;
		if (_poll_fds[i].revents & POLLHUP || _poll_fds[i].revents & POLLERR)
		{
			revent.error = true;
			revent.revent = _poll_fds[i].revents;
			if (i == _poll_fds.size() - 1)
				revent.server = true;
			else
				revent.client_id = i;
			if (i == 0)
				std::cerr << ORANGE << "Server ended connection." << RESET << std::endl;
			else
				std::cerr << ORANGE << "Client " << i << " ended connection." << RESET << std::endl;
			ret.push_back(revent);
			--num_event;
		}
		else if (this->_poll_fds[i].revents & POLLIN)
		{
			revent.error = false;
			revent.revent = POLLIN;
			if (i == this->_poll_fds.size() - 1)
				revent.server = true;
			else
				revent.client_id = i;
			ret.push_back(revent);
			--num_event;
		}
		_poll_fds[i].revents = 0;
	}
	return (ret);
}

bool	ServerCore::pollAvailFor(int idx, nfds_t operation)
{
	int	ret = poll(&_poll_fds[idx], 1, 0);

	if (ret == -1)
	{
		std::cerr << RED << "Poll failed when checking for operation "
			<< operation << " for client " << idx << RESET << std::endl;
		return (false);
	}
	else if (ret == 0)
	{
		//this can be misleading, since timeout is 0 and is asynchronous
		// std::cerr << RED << "Poll timed out for operation "
		// 	<< operation << " for client " << idx << RESET << std::endl;
		return (false);
	}
	return (_poll_fds[idx].revents & operation);
}

bool	ServerCore::_socketCreate()
{
	_server_fd = socket(_config->domain, _config->type, _config->protocol);
	if (_server_fd == SERV_ERROR)
		return (false);
	return (true);
}

bool	ServerCore::_socketSetOptions()
{
	int	opt = 1;

	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == SERV_ERROR)
		return (false);
	return (true);
}

bool	ServerCore::_socketBind()
{
	int	success;

	std::memset(&this->_server_addr, 0, sizeof(this->_server_addr));
	_server_addr.sin_family = _config->domain;
	_server_addr.sin_addr.s_addr = _config->ip;
	_server_addr.sin_port = htons(_config->port_number);
	success = ::bind(_server_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr));
	if (success == SERV_ERROR)
		return (false);
	return (true);
}

bool		ServerCore::_socketListen()
{
	if (listen(_server_fd, _config->incoming_queue_backlog) == SERV_ERROR)
		return (false);
	return (true);
}

void	ServerCore::setNonBlocking(int fd)
{
	int	flags = fcntl(fd, F_GETFL, 0);

	if (flags == SERV_ERROR)
		flags = 0;
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int	ServerCore::_pollWait()
{
	int	poll_count;

	poll_count = poll(&_poll_fds[0], _poll_fds.size(), 0);
	if (poll_count == SERV_ERROR)
		std::cerr << RED << "Poll failed: " << strerror(errno) << RESET << std::endl;
	return (poll_count);
}
