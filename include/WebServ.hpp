/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vviterbo <vviterbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 14:56:52 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/02 13:58:12 by vviterbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "ServerCore.hpp"
#include "Cookie.hpp"
#include "ServerConfig.hpp"

/**
 * @class WebServ
 *
 * @brief High-level server wrapper coordinating clients and the event loop.
 */
class WebServ
{
	public:
		WebServ(const ServerConfig *config);
		~WebServ();

		Client 				*getClient(int uid);
	//SETTERS
	//MEMBER FUNCTIONS
		int					Init();
		int					Run();
		int 				UpdateQueue();
		int 				ProcessQueue();
		Client				*newClient();
		int					removeClient(size_t indx);

	private:
		WebServ(const WebServ &other);
		WebServ	&operator=(const WebServ &other);

		const ServerConfig					*_config;
		ServerCore							*_core;
		std::vector<Client *>				_clients;
		std::deque<Client *>				_processing_queue;
};
