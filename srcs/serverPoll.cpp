/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverPoll.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 14:25:07 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/13 12:31:42 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverPoll.hpp"

serverPoll::serverPoll(Config *config) : _config(config)
{
	this->_poll_fds.resize(this->_config->client_limit + 1);
}

serverPoll::serverPoll(const serverPoll &other) : _poll_fds(other._poll_fds){}

serverPoll &serverPoll::operator=(const serverPoll &other)
{
	if (this != &other)
		this->_poll_fds = other._poll_fds;
	return (*this);
}

serverPoll::~serverPoll() {}

void	serverPoll::pollAdd(int fd, nfds_t event, int indx)
{
	if (indx == -1)
		indx = this->_poll_fds.size() - 1;
    struct pollfd new_poll_fd;
	new_poll_fd.fd = fd;
	new_poll_fd.events = event;
	this->_poll_fds[indx] = new_poll_fd;
}

std::vector<struct pollfd>		serverPoll::getPollFds()
{
	return (this->_poll_fds);
}


void	serverPoll::pollRemove(int indx)
{
	std::memset(&this->_poll_fds[indx], 0, sizeof(pollfd));
}

int		serverPoll::pollWait()
{
	int	poll_count;

	poll_count = poll(&this->_poll_fds[0], this->_poll_fds.size() , 0);
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
	for (unsigned int i = 0; i < this->_poll_fds.size(); ++i)
	{
		if (num_event == 0)
			return (ret);
		if (this->_poll_fds[i].fd == 0)
			continue ;
		if (this->_poll_fds[i].revents & POLLHUP || this->_poll_fds[i].revents & POLLERR)
		{
			revent.error = true;
			revent.revent = this->_poll_fds[i].revents;
			if (i == this->_poll_fds.size() - 1)
				revent.server = true;
			else
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
			revent.error = false;
			revent.revent = POLLIN;
			if (i == this->_poll_fds.size() - 1)
				revent.server = true;
			else
				revent.client_id = i;
			ret.push_back(revent);
			--num_event;
		}
		this->_poll_fds[i].revents = 0;
	}
	return (ret);
}

bool			serverPoll::pollAvailFor(int indx, nfds_t operation)
{
	int		ret = poll(&this->_poll_fds[indx], 1, 0);

	if (ret == -1)
	{
		std::cerr << "Poll failed when checking for operation " << operation << " for client " << indx << std::endl;
		return (false);
	}
	else if (ret == 0)
	{
		std::cerr << "Poll timed out for operation " << operation << " for client " << indx << std::endl;
		return (false);
	}
	else
		return (this->_poll_fds[indx].revents & operation);
}
		
std::ostream	&operator<<(std::ostream &os, serverPoll &poll)
{
	os << "poll stored at " << &poll << " monitor the followings fd\n[";
	std::vector<struct pollfd>	pollfds = poll.getPollFds();
	for (unsigned int i = 0; i < pollfds.size(); ++i)
	{
		if (pollfds[i].fd != 0)
			os << pollfds[i].fd << std::endl;
	}
	os << "]" << std::endl;
	return (os);
}
