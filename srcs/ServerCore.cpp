/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCore.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 15:53:20 by ego               #+#    #+#             */
/*   Updated: 2025/12/02 17:06:29 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCore.hpp"

/**
 * @brief Initializes the server core with config defaults.
 *
 * @param config Configuration to use.
 */
ServerCore::ServerCore(const ServerConfig *config)
{
	_config = config;
	_server_fd = SERV_ERROR;
	std::memset(&_server_addr, 0, sizeof(_server_addr));
	_poll_fds.resize(_config->max_clients * 3 + 1); // clients + server + 2 pipes per client
	for (size_t i = 0; i < _poll_fds.size(); ++i)
	{
		_poll_fds[i].fd = -1;
		_poll_fds[i].events = 0;
		_poll_fds[i].revents = 0;
	}
	return ;
}

/**
 * @brief Copy constructor.
 *
 * @param other Source core.
 */
ServerCore::ServerCore(const ServerCore &other)
{
	_config = other._config;
	_server_fd = other._server_fd;
	_server_addr = other._server_addr;
	_poll_fds = other._poll_fds;
	return ;
}

/**
 * @brief Assignment operator.
 *
 * @param other Source core.
 *
 * @return Reference to this core.
 */
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

/**
 * @brief Destructor closing the listening socket if needed.
 */
ServerCore::~ServerCore()
{
	if (_server_fd > 0)
	{
		close(this->_server_fd);
		this->_server_fd = -1;
	}
	return ;
}

/**
 * @brief Returns the listening socket file descriptor.
 *
 * @return Listening fd.
 */
int	ServerCore::getFd() const
{
	return (this->_server_fd);
}

/**
 * @brief Returns the mutable vector of pollfd structures.
 *
 * @return Reference to pollfd vector.
 */
std::vector<struct pollfd>	&ServerCore::getPollFds()
{
	return (this->_poll_fds);
}

/**
 * @brief Sets up the listening socket (create, options, bind, listen).
 *
 * @return 0 on success, SERV_ERROR on failure.
 */
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

/**
 * @brief Accepts a new client connection and sets non-blocking when needed.
 *
 * @param new_client Client object to fill.
 *
 * @return Accepted socket fd or SERV_ERROR.
 */
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

/**
 * @brief Reads from a client socket if poll marks it readable.
 *
 * @param buffer Destination buffer.
 * @param bytes_read Buffer capacity.
 * @param client Client descriptor owner.
 *
 * @return Bytes read, WBLOCK, or SERV_ERROR.
 */
int	ServerCore::socketRead(char *buffer, int bytes_read, Client *client)
{
	if (!pollAvailFor(client->getId(), POLLIN))
		return (WBLOCK);
	int	bytes_received = recv(client->getFd(), buffer, bytes_read, MSG_DONTWAIT);
	if (bytes_received >= 0)
		std::memset(buffer + bytes_received, 0, bytes_read - bytes_received);
	return (bytes_received);
}

/**
 * @brief Writes to a client socket if poll marks it writable.
 *
 * @param buffer Data to send.
 * @param bytes_write Number of bytes to write.
 * @param client Client descriptor owner.
 *
 * @return Bytes written, WBLOCK, or SERV_ERROR.
 */
int	ServerCore::socketWrite(const char *buffer, int bytes_write, Client *client)
{
	if (!pollAvailFor(client->getId(), POLLOUT))
		return (WBLOCK);
	int	bytes_sent = send(client->getFd(), buffer, bytes_write, MSG_DONTWAIT);
	return (bytes_sent);
}

/**
 * @brief Adds or replaces a pollfd entry for a file descriptor.
 *
 * @param fd File descriptor.
 * @param event Poll events mask.
 * @param idx Index in poll vector (-1 for last).
 */
void	ServerCore::pollAdd(int fd, nfds_t event, int idx)
{
	if (idx == -1)
		idx = _poll_fds.size() - 1;
	struct pollfd	new_poll_fd;
	new_poll_fd.fd = fd;
	new_poll_fd.events = event;
	_poll_fds[idx] = new_poll_fd;
	_poll_fds[idx].revents = 0;
	return ;
}

/**
 * @brief Clears a pollfd entry.
 *
 * @param idx Index to clear.
 */
void	ServerCore::pollRemove(int idx)
{
	_poll_fds[idx].fd = -1;
	_poll_fds[idx].events = 0;
	_poll_fds[idx].revents = 0;
}

/**
 * @brief Polls for events and collects them into a convenience vector.
 *
 * @return Vector of poll events.
 */
std::vector<PollRevent>	ServerCore::pollWatchRevent()
{
	std::vector<PollRevent>	ret;
	PollRevent				revent;
	int						num_event;

	ret.resize(0);
	num_event = _pollWait();
	if (num_event < 0)
		return (ret);
	for (unsigned int i = 0; i < static_cast<unsigned int>(_config->max_clients) + 1; ++i)
	{
		if (i == static_cast<unsigned int>(_config->max_clients))
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

/**
 * @brief Checks whether poll marks an fd ready for the given operation.
 *
 * @param idx Index in poll vector.
 * @param operation Event mask to test (POLLIN/POLLOUT).
 *
 * @return True when ready.
 */
bool	ServerCore::pollAvailFor(int idx, nfds_t operation)
{
	if (idx < 0 || idx >= static_cast<int>(_poll_fds.size()) || _poll_fds[idx].fd < 0)
		return (false);
	int	ret = poll(&_poll_fds[idx], 1, 0);

	if (ret == -1)
	{
		std::cerr << RED << "Poll failed when checking for operation "
			<< operation << " for client " << idx << RESET << std::endl;
		return (false);
	}
	else if (ret == 0)
		return (false);
	return (_poll_fds[idx].revents & operation);
}

/**
 * @brief Creates the listening socket.
 *
 * @return True on success.
 */
bool	ServerCore::_socketCreate()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == SERV_ERROR)
		return (false);
	return (true);
}

/**
 * @brief Applies socket options (currently SO_REUSEADDR).
 *
 * @return True on success.
 */
bool	ServerCore::_socketSetOptions()
{
	int	opt = 1;

	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == SERV_ERROR)
		return (false);
	return (true);
}

/**
 * @brief Binds the listening socket to configured address/port.
 *
 * @return True on success.
 */
bool	ServerCore::_socketBind()
{
	int	success;

	std::memset(&this->_server_addr, 0, sizeof(this->_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = _config->listen_host;
	_server_addr.sin_port = htons(_config->listen_port);
	success = ::bind(_server_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr));
	if (success == SERV_ERROR)
		return (false);
	return (true);
}

/**
 * @brief Starts listening with the configured backlog.
 *
 * @return True on success.
 */
bool		ServerCore::_socketListen()
{
	if (listen(_server_fd, _config->incoming_queue_backlog) == SERV_ERROR)
		return (false);
	return (true);
}

/**
 * @brief Sets a file descriptor to non-blocking mode.
 *
 * @param fd File descriptor.
 */
void	ServerCore::setNonBlocking(int fd)
{
	int	flags = fcntl(fd, F_GETFL, 0);

	if (flags == SERV_ERROR)
		flags = 0;
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/**
 * @brief Waits for events on the poll set (non-blocking timeout 0).
 *
 * @return Number of ready fds or SERV_ERROR.
 */
int	ServerCore::_pollWait()
{
	int	poll_count;

	poll_count = poll(&_poll_fds[0], _poll_fds.size(), 0);
	if (poll_count == SERV_ERROR)
		std::cerr << RED << "Poll failed: " << strerror(errno) << RESET << std::endl;
	return (poll_count);
}
