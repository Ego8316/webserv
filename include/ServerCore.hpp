/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCore.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 14:35:15 by ego               #+#    #+#             */
/*   Updated: 2025/10/17 15:50:52 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "ServerConfig.hpp"

class	Client;
class	ServerConfig;

/**
 * @class ServerCore
 *
 * @brief Handles socket setup and poll monitoring.
 */
class	ServerCore
{
	public:
		ServerCore(const ServerConfig *config);
		ServerCore(const ServerCore &other);
		ServerCore	&operator=(const ServerCore &other);
		~ServerCore();

		int							getFd() const;
		std::vector<struct pollfd>	&getPollFds();

		int						init();
		int						socketAcceptClient(Client *new_client);
		int						socketRead(char *buffer, int bytes_read, Client *client);
		int						socketWrite(const char *buffer, int bytes_write, Client *client);
		void					pollAdd(int fd, nfds_t event, int idx);
		void					pollRemove(int idx);
		std::vector<PollRevent>	pollWatchRevent();
		bool					pollAvailFor(int indx, nfds_t operation);

		static void				setNonBlocking(int fd);
	private:
		const ServerConfig			*_config;
		int							_server_fd;
		struct sockaddr_in			_server_addr;
		std::vector<struct pollfd>	_poll_fds; //[client 0, client 1, ..., client N, server, client 1's pipe to cgi, client 1's pipe from cgi, ..., client N's pipe from cgi] size = 3N + 1

		bool	_socketCreate();
		bool	_socketSetOptions();
		bool	_socketBind();
		bool	_socketListen();
		int		_pollWait();
};
