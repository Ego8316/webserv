/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:17 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/20 19:41:38 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "RequestHandler.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Cookie.hpp"
#include "ServerCore.hpp"

class	Query;
class	Config;
class	Cookie;
class	ServerCore;
class	Request;
class	Response;

class Client
{
	public:
		Client(Config *config, ServerCore *server);
		Client(const Client &other);
		Client	&operator=(const Client &other);
		~Client(void);

		int					getFd();
		struct sockaddr_in	&getClientAddr();
		socklen_t			&getClientLen();
		int					getId();
		RequestStage		getState();

		void	setFd(int fd);
		void	setClientId(int id);
		void    setState(RequestStage state);
		int		handleEvent();

	private:
		Config				*_config;
		ServerCore			*_server;
		int					_client_fd;
		struct sockaddr_in	_client_addr;
		socklen_t			_client_len;
		int					_client_id;
		RequestStage		_state;
		long				_time_limit;
		Request				*_request;
		Response			_response;

		int		_tryAccepting();
		int		_readInput();
		void	_processRequest();
		int		_sendOutput();
};
