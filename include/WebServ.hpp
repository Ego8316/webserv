/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 14:56:52 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/28 22:54:15 by victorviter      ###   ########.fr       */
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

		int					Init();
		int					Run();
		Client 				*getClient(int uid);
		int 				updateQueue();
		int 				_processQueue();
		void				sendTimeOut(Client *client);

	private:
		Client				*_newClient();
		int					_removeClient(size_t indx);
		
		const ServerConfig					*_config;
		ServerCore							*_core;
		std::vector<Client *>				_clients;
		std::deque<Client *>				_processing_queue;
};
