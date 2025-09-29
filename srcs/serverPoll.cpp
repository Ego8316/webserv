/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverPoll.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 14:25:07 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/29 14:25:07 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverPoll.hpp"

serverPoll::serverPoll()
{
	this->_poll_fds.resize(this->_poll_count);
}

serverPoll::serverPoll(const serverPoll &other) : _poll_fds(other._poll_fds){}

serverPoll &serverPoll::operator=(const serverPoll &other)
{
	if (this != &other)
		this->_poll_fds = other._poll_fds;
	return (*this);
}

serverPoll::~serverPoll() {}

void	serverPoll::pollAdd(int fd, int event, int indx)
{
    struct pollfd new_poll_fd;
	new_poll_fd.fd = fd;
	new_poll_fd.events = event;
	this->_poll_fds[indx] = new_poll_fd;
}

void	serverPoll::pollRemove(int indx)
{
	std::memset(&this->_poll_fds[indx], 0, sizeof(pollfd));
}

int		serverPoll::pollWait(int time_Out)
{
	int	poll_count;

	poll_count = poll(&this->_poll_fds[0], this->_poll_count, time_Out);
	if (poll_count == -1)
		std::cerr << "Poll failed: " << strerror(errno) << std::endl;
	return (poll_count);
}

int		serverPoll::pollWatchRevent(Config &config)
{
	static unsigned int	i = 0;

	this->pollWait(config.time_out);
	for (i = 0; i < this->_poll_count; ++i)
	{
		if (this->_poll_fds[i].revents & POLLHUP || this->_poll_fds[i].revents & POLLERR)
		{
			if (i == 0)
			{
				std::cerr << "Server ended connection." << std::endl;
				return (SERV_ERROR);
			}
			else
			{
				std::cerr << "Client ended connection." << std::endl;
				return (CLIENT_ERR_IDX(i));	
			}
		}
        else if (this->_poll_fds[i].revents & POLLIN)
		{
			if (i == 0)
				return (NEW_CLIENT);
			else
				return (i);
		}
	}
	return (0);
}
