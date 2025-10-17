/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:17 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/17 17:59:41 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "RequestHandler.hpp"
#include "Config.hpp"
#include "Cookie.hpp"
#include "ServerCore.hpp"

class	Query;
class	Config;
class	Cookie;
class	ServerCore;

class	Client
{
	public:
		Client(Config *config, std::map<std::string, Cookie *> *all_cookies, ServerCore *core);
		Client(const Client &other);
		Client	&operator=(const Client &other);
		~Client(void);

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
		int								_client_fd;
		struct sockaddr_in				_client_addr;
		socklen_t						_client_len;
		int								_client_id;
		std::map<std::string, Cookie *> *_all_cookies;
		Config							*_config;
		ServerCore						*_core;
};
