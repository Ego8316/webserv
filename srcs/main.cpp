/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hcavet <hcavet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 10:44:51 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/04 17:20:43 by hcavet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"
#include "ServerConfig.hpp"
#include "WebServ.hpp"
#include "ConfigLexer.hpp"
#include "ConfigParser.hpp"
#include "ConfigInterpreter.hpp"
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

/**
 * @brief Read a whole file into a string.
 *
 * @throws std::runtime_error when the file cannot be opened.
 * 
 * @param path Filesystem path to read.
 *
 * @return File contents.
 */
static std::string	readFile(const std::string &path)
{
	std::ifstream		file(path.c_str());
	if (!file.is_open())
	{
		if (access(path.c_str(), F_OK) == -1)
			throw std::runtime_error("File not found: " + path);
		if (access(path.c_str(), R_OK) == -1)
			throw std::runtime_error("Permission denied: " + path);
		throw std::runtime_error("Could not open: " + path);
	}
	struct stat st;
	if (stat(path.c_str(), &st) == -1)
		throw std::runtime_error("Cannot stat path: " + path + " (" + strerror(errno) + ")");
	if (S_ISDIR(st.st_mode))
		throw std::runtime_error("Path is a directory: " + path);
	std::stringstream	buffer;
	buffer << file.rdbuf();
	const std::string content = buffer.str();
	if (content.empty())
		throw std::runtime_error("Configuration file is empty: " + path);
	return (content);
}

/**
 * @brief Parse a configuration file into server configs.
 *
 * @param path Path to the config file.
 *
 * @return Vector of interpreted ServerConfig objects.
 */
static std::vector<ServerConfig>	parseFile(const std::string &path)
{
	const std::string		&input = readFile(path);
	ConfigLexer				lexer(input);
	std::vector<Token>		tokens = lexer.tokenize();
	ConfigParser			parser(tokens);
	std::vector<Block>		blocks = parser.parse();
	ConfigInterpreter		interpreter(blocks);
	return (interpreter.interpret());
}

/**
 * @brief Release allocated WebServ instances and clear the vector.
 *
 * @param servers Vector of server pointers to delete.
 */
static void	destroyServers(std::vector<WebServ*> &servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		delete servers[i];
		servers[i] = NULL;
	}
	servers.clear();
}

/**
 * @brief Program entry point: parse config, start servers, run loop.
 *
 * Expects a single argument: the configuration file path.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 *
 * @return 0 on success, 1 on usage error or runtime failures.
 */
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
		configs = parseFile(argv[1]);
		if (configs.empty())
			throw std::runtime_error("No server blocks found");
		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);
		signal(SIGHUP, signal_handler);
		for (size_t i = 0; i < configs.size(); ++i)
		{
			web_servers.push_back(new WebServ(&configs[i]));
			if (web_servers.back()->Init() == SERV_ERROR)
				throw std::runtime_error("Init failed for server " + utils::toString(i));
		}
		while (!g_shutdown)
			for (size_t i = 0; i < web_servers.size(); ++i)
				if (web_servers[i]->Run() == SERV_ERROR)
					throw std::runtime_error("Server " + utils::toString(i) + ": fatal error occured");
	}
	catch (const std::exception &e) { std::cerr << BOLD_RED << "Error: " << RED << e.what() << RESET << std::endl; }
	destroyServers(web_servers);
	return (0);
}
