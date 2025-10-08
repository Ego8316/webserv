/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 10:44:51 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/08 14:05:09 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "WebServ.hpp"


int main(int argc, char *argv[])
{
	std::vector<Config *>				configs;
	std::vector<WebServ *>				web_servers;
	std::vector<WebServ *>::iterator	it_web;
	std::vector<Config *>::iterator		it_conf;
	
	if (argc != 2)
	{
		std::cerr << "Please provide config file" << std::endl;
		return (1);
	}
	configs = Config::parseMultipleConfigs(argv[1]);
	if (configs.size() == 0)
		return (1);
	for (unsigned int i = 0; i < configs.size(); ++i)
		web_servers.push_back(new WebServ(configs[i]));
	for (unsigned int i = 0; i < web_servers.size(); ++i)
	{
		if (web_servers[i]->WebServInit() == SERV_ERROR)
		{
			delete web_servers[i];
			it_web = web_servers.begin() + i;
			web_servers.erase(it_web);
			delete configs[i];
			it_conf = configs.begin() + i;
			configs.erase(it_conf);
			std::cerr << "Could not init server " << i << std::endl;
		}
		
	}
	while (true)
	{
		for (unsigned int i = 0; i < web_servers.size(); ++i)
		{
			if (web_servers[i]->WebServRun() == SERV_ERROR)
			{
				if (web_servers[i]->WebServReboot() == SERV_ERROR)
				{
					delete web_servers[i];
					it_web = web_servers.begin() + i;
					web_servers.erase(it_web);
					delete configs[i];
					it_conf = configs.begin() + i;
					configs.erase(it_conf);
					std::cerr << "Server " << i << ": fatal error occured" << std::endl;
				}
			}
		}
	}
	return (0);
}