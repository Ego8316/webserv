/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:17 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 21:41:58 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Query.hpp"
#include "Config.hpp"

class Query;
class Config;

class Client {
	public :
	// CONSTRUCTORS
		Client(Config *config);
		Client(const Client &other);
		Client &operator=(const Client &other);
	//DESTUCTORS
		~Client();
	//GETTERS
		int					getFd();
		struct sockaddr_in	&getClientAddr();
		socklen_t			&getClientLen();
	//SETTERS
		void				setFd(int fd);
		void				setClientId(int id);
	//MEMBER FUNCTIONS
		int					handleEvent();
		int					socketRead(char *buffer, int bytes_read);
		int					socketWrite(const char *buffer, int bytes_write);
	private :
		int					_client_fd;
		struct sockaddr_in	_client_addr;
		socklen_t			_client_len;
		int					_client_id;
		Config				*_config;
};
