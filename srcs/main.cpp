/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 10:44:51 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/10 13:57:58 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "WebServ.hpp"

void	deleteAllConfigs(std::vector<Config *> &configs)
{
	for (unsigned int i = 0; i < configs.size(); ++i)
	{
		if (configs[i] != NULL)
			delete configs[i];
	}
	configs.resize(0);
}

std::vector<Config *> parseMultipleConfigs(std::string filename)
{
	std::ifstream 			conf_file;
	std::string				newline;
	std::vector<Config *>	configs;
	std::string				config_section;
	
	conf_file.open(filename.c_str(), std::ios::in);
	
	configs.resize(0);
	if (!conf_file.is_open())
	{
		std::cerr << "Could not open config file" << std::endl;
		return (configs);
	}
	while (std::getline(conf_file, newline))
	{
		if (newline.find("{") != std::string::npos)
		{
			if (config_section.length() != 0)
			{
				std::cerr << "Nested configs not supported" << std::endl;
				deleteAllConfigs(configs);
				return (configs);
			}
			else if (newline.find("}") != std::string::npos)
			{
				std::cerr << "Format not recognized" << std::endl;
				deleteAllConfigs(configs);
				return (configs);
			}
			else
				config_section = newline.erase(0, newline.find("{") + 1) + "\n";
		}
		else if (newline.find("}") != std::string::npos)
		{
			config_section += newline.substr(0, newline.find("}")) + "\n";
			configs.push_back(new Config(config_section));
			std::cout << *configs[0] << std::endl;
			config_section = "";
		}
		else if (newline.length())
			config_section += newline + "\n";
	}
	conf_file.close();
	return (configs);
}

int main(int argc, char *argv[])
{
	std::vector<Config *>				configs;
	std::vector<WebServ *>				web_servers;
	std::vector<WebServ *>::iterator	it_web;
	std::vector<Config *>::iterator		it_conf;
	
	if (argc == 1)
		configs = parseMultipleConfigs("default.config");
	else if (argc == 2)
		configs = parseMultipleConfigs(argv[1]);
	else
	{
		std::cerr << "Please provide one config file" << std::endl;
		return (1);
	}
	if (configs.size() == 0)
		return (1);
	std::cout << "Found " << configs.size() << " configurations" << std::endl;
	for (unsigned int i = 0; i < configs.size(); ++i)
		web_servers.push_back(new WebServ(configs[i]));
	for (unsigned int i = 0; i < web_servers.size(); ++i)
	{
		if (web_servers[i]->WebServInit() == SERV_ERROR)
		{
			std::cout << "WEBSERVER ERROR !!!!!" << std::endl;
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
				std::cout << "WEBSERVER ERROR !!!!!" << std::endl;
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