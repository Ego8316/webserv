/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:17 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/19 14:33:33 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "RequestHandler.hpp"
#include "Config.hpp"
#include "Cookie.hpp"
#include "serverPoll.hpp"

class Query;
class Config;
class Cookie;
class serverPoll;

class Client {
	public :
	// CONSTRUCTORS
		Client(Config *config, serverPoll *poll);
		Client(const Client &other);
		Client &operator=(const Client &other);
	//DESTUCTORS
		~Client();
	//GETTERS
		int					getFd();
		struct sockaddr_in	&getClientAddr();
		socklen_t			&getClientLen();
		RequestStage		getState();
	//SETTERS
		void				setFd(int fd);
		void				setClientId(int id);
		void    			setState(RequestStage state);
	//MEMBER FUNCTIONS
		int					handleEvent();
		int					socketRead(char *buffer, int bytes_read);
		int					socketWrite(const char *buffer, int bytes_write);
	private :
		int								_client_fd;
		struct sockaddr_in				_client_addr;
		socklen_t						_client_len;
		int								_client_id;
		
		RequestStage					_state;
		std::string						_preprend_response;
		int								_response_fd;
		int								_cgi_pid;
		
		Config							*_config;
		serverPoll						*_poll;
};
