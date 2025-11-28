/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 10:44:51 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/27 04:19:44 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"
#include "ServerConfig.hpp"
#include "WebServ.hpp"
#include "ConfigLexer.hpp"
#include "ConfigParser.hpp"
#include "ConfigInterpreter.hpp"
#include "debug.hpp"
#include "utils.hpp"

int g_shutdown = 0;

/**
 * @brief Signal handler to request graceful shutdown.
 *
 * @param signal Caught signal.
 */
void	signal_handler(int signal)
{
	(void)signal;
	g_shutdown = 1;
}

std::string	readFile(const std::string &path)
{
	std::ifstream		file(path.c_str());
	if (!file.is_open())
		throw std::runtime_error("Could not open: " + path);
	std::stringstream	buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

static void	destroyServers(std::vector<WebServ*> &servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		delete servers[i];
		servers[i] = NULL;
	}
	servers.clear();
}

int	main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << RED << "Usage: " << argv[0] << " <config file>" << RESET << std::endl;
		return (1);
	}
	std::vector<WebServ*>		web_servers;
	std::vector<ServerConfig>   configs;
	try
	{
		const std::string		&input = readFile(argv[1]);
		ConfigLexer				lexer(input);
		std::vector<Token>		tokens = lexer.tokenize();
		ConfigParser			parser(tokens);
		std::vector<Block>		blocks = parser.parse();
		ConfigInterpreter		interpreter(blocks);
		configs = interpreter.interpret();

		if (configs.empty())
			throw std::runtime_error("No server blocks found");
		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);
		signal(SIGHUP, signal_handler);
		ServerCore::setNonBlocking(STDIN_FILENO);

		for (size_t i = 0; i < configs.size(); ++i)
		{
			web_servers.push_back(new WebServ(&configs[i]));
			if (web_servers.back()->Init() == SERV_ERROR)
				throw std::runtime_error("Init failed for server " + utils::toString(i));
		}
		while (!g_shutdown)
		{
			for (size_t i = 0; i < web_servers.size(); ++i)
			{
				if (web_servers[i]->Run() == SERV_ERROR)
				{
					std::cerr << RED << "Server " << i << ": fatal error occured" << RESET << std::endl;
					g_shutdown = 1;
					break ;
				}
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
	}
	destroyServers(web_servers);
	return (0);
}

/**
 * @brief Program entry point: loads configs, starts servers, runs event loop.
 *
 * @param argc Argument count.
 * @param argv Argument values.
 *
 * @return Exit status.
 */
// int main(int argc, char *argv[])
// {
// 	std::vector<Config *>				configs;
// 	std::vector<WebServ *>				web_servers;
	
// 	signal(SIGINT, signal_handler);
// 	signal(SIGTERM, signal_handler);
// 	signal(SIGHUP, signal_handler);
// 	if (argc == 2)
// 		configs = parseConfigFile(argv[1]);
// 	else
// 	{
// 		std::cerr << RED << "Please provide only one config file" << RESET << std::endl;
// 		return (1);
// 	}
// 	if (configs.size() == 0)
// 		return (1);
// 	ServerCore::setNonBlocking(STDIN_FILENO);
// 	for (unsigned int i = 0; i < configs.size(); ++i)
// 		web_servers.push_back(new WebServ(configs[i]));
// 	for (unsigned int i = 0; i < web_servers.size(); ++i)
// 	{
// 		if (web_servers[i]->Init() == SERV_ERROR)
// 		{
			
// 			std::cerr << "Could not init server " << i << std::endl;
// 		}
// 	}
// 	while (!g_shutdown)
// 	{
// 		for (unsigned int i = 0; i < web_servers.size(); ++i)
// 		{
// 			if (web_servers[i]->Run() == SERV_ERROR)
// 			{
// 				deleteServer(web_servers, configs, i);
// 				std::cerr << RED << "Server " << i << ": fatal error occured" << RESET << std::endl;
// 			}
// 		}
// 	}
// 	shutdown(web_servers, configs);
// 	return (0);
// }
