/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:17 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/24 23:40:30 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Response.hpp"
#include "ServerConfig.hpp"
#include "ServerCore.hpp"
#include "RequestHandler.hpp"

class	Response;
class	Query;
class	ServerConfig;
class	ServerCore;
class	Request;

/**
 * @class Client
 *
 * @brief Tracks a connected client state machine and its request/response.
 */
class Client
{
	public:
		Client(const ServerConfig *config, ServerCore *server);
		Client(const Client &other);
		Client	&operator=(const Client &other);
		~Client();

		int						handleEvent();
		int						getFd();
		struct sockaddr_in		&getClientAddr();
		socklen_t				&getClientLen();
		int						getId() const;
		RequestStage			getState();
		ServerCore				&getServer();
		long					getTimeLimit();
		long					getRequestTimeLimit();
		void					setFd(int fd);
		void					setClientId(int id);
		void    				setState(RequestStage state);

		void	printState() const;
		void	printHeader() const;
		void	printRequest() const;

	private :
		const ServerConfig				*_config;
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
		bool							_keep_alive;
		Request							*_request;
		Response						*_response;
		
		int		_tryAccepting();
		int		_requestInit();
		int		_readHeader();
		int		_readBody();
		void	_processRequest();
		int		_sendString();
		int		_sendFile();
		void	_prepareNew();
		void	_monitorCGI();
};
