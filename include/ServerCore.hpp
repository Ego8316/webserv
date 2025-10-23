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
#include "Config.hpp"

class	Client;
class	Config;

/**
 * @class Handles socket setup and poll monitoring.
 */
class	ServerCore
{
	public:
		ServerCore(const Config *config);
		ServerCore(const ServerCore &other);
		ServerCore	&operator=(const ServerCore &other);
		~ServerCore(void);

		int							getFd(void) const;
		std::vector<struct pollfd>	&getPollFds(void);

		int						init(void);
		int						socketAcceptClient(Client *new_client);
		int						socketRead(char *buffer, int bytes_read, Client *client);
		int						socketWrite(const char *buffer, int bytes_write, Client *client);
		void					pollAdd(int fd, nfds_t event, int idx);
		void					pollRemove(int idx);
		std::vector<pollRevent>	pollWatchRevent(void);
		bool					pollAvailFor(int indx, nfds_t operation);

	private:
		const Config				*_config;
		int							_server_fd;
		struct sockaddr_in			_server_addr;
		std::vector<struct pollfd>	_poll_fds;

		bool	_socketCreate(void);
		bool	_socketSetOptions(void);
		bool	_socketBind(void);
		bool	_socketListen(void);
		void	_setNonBlocking(int fd);
		int		_pollWait(void);

};
