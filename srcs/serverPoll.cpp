/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverPoll.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 14:25:07 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/08 14:29:27 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverPoll.hpp"

serverPoll::serverPoll(Config *config) : _config(config)
{
	this->_poll_fds.resize(this->_config->client_limit);
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

int		serverPoll::pollWait()
{
	int	poll_count;

	poll_count = poll(&this->_poll_fds[0], this->_config->client_limit, 0);
	if (poll_count == -1)
		std::cerr << "Poll failed: " << strerror(errno) << std::endl;
	return (poll_count);
}

std::vector<pollRevent>	serverPoll::pollWatchRevent()
{
	std::vector<pollRevent>		ret;
	pollRevent					revent;
	int							num_event;

	ret.resize(0);
	num_event = this->pollWait();
	if (num_event < 0)
		return (ret);
	for (int i = 0; i < this->_config->client_limit; ++i)
	{
		if (num_event == 0)
			return (ret);
		if (this->_poll_fds[i].revents & POLLHUP || this->_poll_fds[i].revents & POLLERR)
		{
			revent.is_error = true;
			revent.revent = this->_poll_fds[i].revents;
			revent.client_id = i;
			if (i == 0)
				std::cerr << "Server ended connection." << std::endl;
			else
				std::cerr << "Client " << i << " ended connection." << std::endl;
			ret.push_back(revent);
			--num_event;
		}
        else if (this->_poll_fds[i].revents & POLLIN)
		{
			revent.is_error = false;
			revent.revent = POLLIN;
			revent.client_id = i;
			ret.push_back(revent);
			--num_event;
		}
		this->_poll_fds[i].revents = 0;
	}
	return (ret);
}
