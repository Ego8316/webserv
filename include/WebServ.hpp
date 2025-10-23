/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 14:56:52 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/23 11:59:30 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "ServerCore.hpp"
#include "Cookie.hpp"
#include "Config.hpp"

class WebServ
{
	public:
		WebServ(Config *config);
		WebServ(std::string config_file);
		WebServ(const WebServ &other);
		WebServ	&operator=(const WebServ &other);
		~WebServ();

		Client 				*getClient(int uid);
	//SETTERS
	//MEMBER FUNCTIONS
		int					Init();
		int					Run();
		int 				UpdateQueue();
		int 				ProcessQueue();
		Client				*newClient();
		int					removeClient(int indx);
	private :
		Config								*_config;
		ServerCore							*_core;
		std::vector<Client *>				_clients;
		std::deque<Client *>				_processing_queue;
};
