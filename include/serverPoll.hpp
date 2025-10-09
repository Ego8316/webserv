/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverPoll.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 14:25:15 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/08 14:56:40 by victorviter      ###   ########.fr       */
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

class serverPoll{
	public :
	// CONSTRUCTORS
		serverPoll(Config *config);
		serverPoll(const serverPoll &other);
		serverPoll &operator=(const serverPoll &other);
	//DESTUCTORS
		~serverPoll();
	//GETTERS
	//SETTERS
	//MEMBER FUNCTIONS
		void						pollAdd(int fd, int event, int indx);
		void						pollRemove(int indx);
		int							pollWait();
		std::vector<pollRevent>		pollWatchRevent();
	private :
		std::vector<struct pollfd>			_poll_fds;
		Config								*_config;
};
