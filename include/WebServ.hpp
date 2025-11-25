/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 14:56:52 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/24 23:41:12 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "ServerCore.hpp"
#include "Cookie.hpp"
#include "Config.hpp"

/**
 * @class WebServ
 *
 * @brief High-level server wrapper coordinating clients and the event loop.
 */
class WebServ
{
	public:
		WebServ(const Config *config);
		~WebServ();

		Client 				*getClient(int uid);
	//SETTERS
	//MEMBER FUNCTIONS
		int					Init();
		int					Run();
		int 				UpdateQueue();
		int 				ProcessQueue();
		Client				*newClient();
		int					removeClient(int indx);

	private:
		WebServ(const WebServ &other);
		WebServ	&operator=(const WebServ &other);

		const Config						*_config;
		ServerCore							*_core;
		std::vector<Client *>				_clients;
		std::deque<Client *>				_processing_queue;
};
