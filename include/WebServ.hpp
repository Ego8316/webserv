/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 14:56:52 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/10 13:45:22 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "serverSocket.hpp"
#include "serverPoll.hpp"
#include "Cookie.hpp"
#include "Config.hpp"

class WebServ {
	public :
	// CONSTRUCTORS
		WebServ(Config *config);
		WebServ(std::string config_file);
		WebServ(const WebServ &other);
		WebServ &operator=(const WebServ &other);
	//DESTUCTORS
		~WebServ();
	//GETTERS
		Client 				*getClient(int uid);
	//SETTERS
	//MEMBER FUNCTIONS
		int					WebServInit();
		int					WebServRun();
		int					newClient();
		int					removeClient(int indx);
		int					WebServReboot();
	private :
		Config								*_config;
		serverSocket						*_server;
		serverPoll							*_poll;
		std::map<std::string, Cookie *>		*_cookie_sessions;
		std::vector<Client *>				_clients;
};
