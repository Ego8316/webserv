/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 14:56:52 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/17 17:55:12 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "ServerCore.hpp"
#include "Cookie.hpp"
#include "Config.hpp"

class WebServ
{
	public:
		WebServ(Config *config);
		WebServ(std::string config_file);
		WebServ(const WebServ &other);
		WebServ	&operator=(const WebServ &other);
		~WebServ();

		Client 				*getClient(int uid);

		int	WebServInit();
		int	WebServRun();
		int	newClient();
		int	removeClient(int indx);
		int	WebServReboot();

	private :
		Config							*_config;
		ServerCore						*_core;
		std::map<std::string, Cookie *>	*_cookie_sessions;
		std::vector<Client *>			_clients;
};
