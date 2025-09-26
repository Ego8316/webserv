/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverPoll.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 14:25:15 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/25 22:37:56 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "serverSocket.hpp"
#include "clientSocket.hpp"

class clientSocket;
class serverSocket;

//serverPoll contains all the variables and function that pertains to the poll
//function which is used to monitor fd activity and availability for I/O operations
//see I/O multiplexing
//first element of vector will always be server fd which will not change
//following elements are clients fds that are dynamically created and destroyed

class serverPoll {
	public :
	// CONSTRUCTORS
		serverPoll();
		serverPoll(const serverPoll &other);
		serverPoll &operator=(const serverPoll &other);
	//DESTUCTORS
		~serverPoll();
	//GETTERS
		serverSocket	*getServSocket();
	//SETTERS
		void			setServSocket(serverSocket *server);
	//MEMBER FUNCTIONS
		void	pollAdd(int fd, int event, clientSocket *client);
		int		pollRemove(int indx);
		int		pollWait(int time_Out);
		int		pollWatchRevent();
	private :
		static const unsigned int			_poll_count = 10000;	//number of fds simultaneously handled by poll (different from backlog which is number of connections pending 'accept')
		std::vector<struct pollfd>			_poll_fds;				//vector of fds (will be of len _poll_count)
		serverSocket						*_server;
		std::map<int, clientSocket *>		_client_map;
};
