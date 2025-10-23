/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:17 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/23 12:31:04 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Cookie.hpp"
#include "ServerCore.hpp"
#include "RequestHandler.hpp"

class	Response;
class	Query;
class	Config;
class	Cookie;
class	ServerCore;
class	Request;

class Client
{
	public:
		Client(Config *config, ServerCore *server);
		Client(const Client &other);
		Client	&operator=(const Client &other);
		~Client(void);

		int						handleEvent();
		int						getFd();
		struct sockaddr_in		&getClientAddr();
		socklen_t				&getClientLen();
		int						getId();
		RequestStage			getState();
		ServerCore				&getServer();
		long					getTimeLimit();
		void					setFd(int fd);
		void					setClientId(int id);
		void    				setState(RequestStage state);

		void	printState() const;
		void	printHeader() const;
		void	printRequest() const;

	private :
		Config							*_config;
		ServerCore						*_server;
		int								_client_fd;
		struct sockaddr_in				_client_addr;
		socklen_t						_client_len;
		int								_client_id;
		RequestStage					_state;
		ProcessError					_error;
		std::string						_leftover;
		int								_bytes_sent;
		int								_bytes_in_buffer;
		long							_time_limit;
		long							_request_time_limit;
		Request							*_request;
		Response						*_response;
		
		int		_tryAccepting();
		int		_readHeader();
		int		_readBody();
		void	_processRequest();
		int		_sendString();
		int		_sendFile();
		void	_prepareNew();
		int		_monitorCGI();
};
