/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 10:44:51 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/29 16:33:56 by victorviter      ###   ########.fr       */
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
		std::cerr << RED << "Destroying server and config " << i + 1 << "/" << web_servers.size() << RESET << std::endl;
		delete web_servers[i];
		delete configs[i];
	}
	web_servers.clear();
	configs.clear();
}

std::vector<Config *> parseConfigFile(std::string filename)
{
	std::ifstream 			conf_file(filename.c_str());
	std::vector<Config *>	configs;
	std::string				line, block_content, current_name;
	bool					in_server_block = false;
	int						brace_depth = 0;

	if (!conf_file.is_open())
		throw Config::Error("Could not open config file: " + filename);
	try
	{
		while (std::getline(conf_file, line))
		{
			line = utils::stringTrimSpaces(line);
			if (!in_server_block && (line.empty() || line[0] == '#'))
			{
				Config::line_number++;
				continue ;
			}
			if (!in_server_block && utils::caseInsensitiveFind(line, "server") != line.end())
			{
				size_t	name_start = std::string("server").length();
				size_t	brace_pos = line.find("{", name_start);

				if (name_start >= line.size() || !isspace(line[name_start]))
					throw Config::Error("Unknown keyword: expected `server'");
				if (brace_pos == std::string::npos)
					throw Config::Error("Missing `{' after server declaration");
				if (brace_pos != line.size() - 1)
					throw Config::Error("Unexpected token after `{' in server declaration");
				current_name = line.substr(name_start, brace_pos - name_start);
				utils::stringTrimSpaces(current_name);
				in_server_block = true;
				block_content.clear();
				brace_depth = 1;
				continue ;
			}
			if (in_server_block)
			{
				for (size_t i = 0; i < line.size(); ++i)
				{
					if (line[i] == '{') brace_depth++;
					else if (line[i] == '}') brace_depth--;
				}
				if (brace_depth == 0)
				{
					in_server_block = false;
					try
					{
						Config	*cfg = new Config(block_content, current_name);
						configs.push_back(cfg);
					}
					catch (const Config::Error &e) { throw; }
				}
				block_content += line + "\n";
			}
			else
				throw Config::Error("Unexpected content oustside server block");
		}
		if (in_server_block)
			throw Config::Error("Unmatched `{' before end of file");
	}
	catch (const Config::Error &e)
	{
		deleteAllConfigs(configs);
		std::cerr << BOLD_RED << "Configuration error: " << RED << e.what() << " at line " << Config::line_number << RESET << std::endl;
	}
	conf_file.close();
	return (configs);
}

int main(int argc, char *argv[])
{
	std::vector<Config *>				configs;
	std::vector<WebServ *>				web_servers;
	
	signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
	if (argc == 2)
		configs = parseConfigFile(argv[1]);
	else
	{
		std::cerr << RED << "Please provide only one config file" << RESET << std::endl;
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
	while (!g_shutdown)
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
	return (0);
}
