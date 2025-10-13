/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverPoll.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 14:25:15 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/13 12:22:39 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "serverSocket.hpp"
#include "Client.hpp"
#include "Config.hpp"

class Client;
class serverSocket;
class Config;

class serverPoll
{
	public :
	// CONSTRUCTORS
		serverPoll(Config *config);
		serverPoll(const serverPoll &other);
		serverPoll &operator=(const serverPoll &other);
	//DESTUCTORS
		~serverPoll();
	//GETTERS
		std::vector<struct pollfd>			getPollFds();
	//SETTERS
	//MEMBER FUNCTIONS
		void						pollAdd(int fd, nfds_t event, int indx);
		void						pollRemove(int indx);
		int							pollWait();
		std::vector<pollRevent>		pollWatchRevent();
		bool						pollAvailFor(int indx, nfds_t operation);
	private :
		std::vector<struct pollfd>			_poll_fds;
		Config								*_config;
};

std::ostream	&operator<<(std::ostream &os, serverPoll &poll);
