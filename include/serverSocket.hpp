/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 13:40:49 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/28 19:04:17 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"

class Client;
//serverSocket contains all the variables and functions that pertains to socket handling
//including initialisation, setting up the addresses of the client and server,
//accepting, listening and ending connections

class serverSocket {
	public :
	// CONSTRUCTORS
		serverSocket();
		serverSocket(const serverSocket &other);
		serverSocket &operator=(const serverSocket &other);
	//DESTUCTORS
		~serverSocket();
	//GETTERS
		int					getFd();
	//SETTERS
	//MEMBER FUNCTIONS
		int					socketInit(Config &config);
		int					socketBind(int portNumber);
		int					socketAcceptClient(Client *new_client);
		int					socketListen(Config config);
		int					setSockOpt();
	private :
		static const unsigned int	_backlog = 100;		//TODO get from config
		int							_domain;			//TODO get from config
		int							_type;				//TODO get from config
		int							_protocol;			//TODO get from config
   		int							_server_fd;			//server file descriptor
		struct sockaddr_in			_server_addr;
};
