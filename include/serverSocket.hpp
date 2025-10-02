/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 13:40:49 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/02 09:08:01 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "Config.hpp"

class Client;
class Config;
//serverSocket contains all the variables and functions that pertains to socket handling
//including initialisation, setting up the addresses of the client and server,
//accepting, listening and ending connections

class serverSocket {
	public :
	// CONSTRUCTORS
		serverSocket(Config *config);
		serverSocket(const serverSocket &other);
		serverSocket &operator=(const serverSocket &other);
	//DESTUCTORS
		~serverSocket();
	//GETTERS
		int					getFd();
	//SETTERS
	//MEMBER FUNCTIONS
		int					socketBind();
		int					socketAcceptClient(Client *new_client);
		int					socketListen();
		int					setSockOpt();
	private :
   		int							_server_fd;
		struct sockaddr_in			_server_addr;
		Config						*_config;
};
