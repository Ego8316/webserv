/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 10:44:51 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/24 16:51:42 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "WebServ.hpp"

int g_shutdown = 0;

void	deleteAllConfigs(std::vector<Config *> &configs)
{
	for (unsigned int i = 0; i < configs.size(); ++i)
	{
		if (configs[i] != NULL)
			delete configs[i];
	}
	configs.resize(0);
}

void	signal_handler(int signal)
{
	(void)signal;
	g_shutdown = 1;
}

void	deleteServer(std::vector<WebServ *> &web_servers, std::vector<Config *> &configs, int i)
{
	std::vector<WebServ *>::iterator	it_web;
	std::vector<Config *>::iterator		it_conf;

	delete web_servers[i];
	it_web = web_servers.begin() + i;
	web_servers.erase(it_web);
	delete configs[i];
	it_conf = configs.begin() + i;
	configs.erase(it_conf);
	return ;
}

void	shutdown(std::vector<WebServ *> &web_servers, std::vector<Config *> &configs)
{
	unsigned int len = web_servers.size();
	for (unsigned int i = 0; i < len; ++i)
	{
		std::cerr << RED << "Destroying server and config " << i << "/" << web_servers.size() << RESET << std::endl;
		delete web_servers[i];
		delete configs[i];
	}
	web_servers.clear();
	configs.clear();
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
	
	signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTERM, signal_handler);  // kill command or system shutdown
    signal(SIGHUP, signal_handler);   // Terminal hangup
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
	ServerCore::setNonBlocking(STDIN_FILENO);
	for (unsigned int i = 0; i < configs.size(); ++i)
		web_servers.push_back(new WebServ(configs[i]));
	for (unsigned int i = 0; i < web_servers.size(); ++i)
	{
		if (web_servers[i]->Init() == SERV_ERROR)
		{
			
			std::cerr << "Could not init server " << i << std::endl;
		}
	}
	long start = utils::getTime();
	while (!g_shutdown && utils::getTime() < start + 10)
	{
		for (unsigned int i = 0; i < web_servers.size(); ++i)
		{
			if (web_servers[i]->Run() == SERV_ERROR)
			{
				deleteServer(web_servers, configs, i);
				std::cerr << RED << "Server " << i << ": fatal error occured" << RESET << std::endl;
			}
		}
	}
	shutdown(web_servers, configs);
	return (123);
}

