/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:46 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/14 11:41:54 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

cgi::cgi() {}

cgi::cgi(const cgi &other)
{
	(void)other;
}

cgi &cgi::operator=(const cgi &other)
{
	(void)other;
	return (*this);
}

cgi::~cgi() {}


void		cgi::cgiRun(Client &client, Request &request, Config &config, std::vector<Cookie *> cookies)
{
	int		pipe_to_CGI[2];
	int		pipe_from_CGI[2];
	pid_t	pid[2];

	if (pipe(pipe_to_CGI) == -1)
	{
		std::cerr << "Could not initialize pipe " << strerror(errno) << std::endl;
		return ;
	}
	if (pipe(pipe_from_CGI) == -1)
	{
		std::cerr << "Could not initialize pipe " << strerror(errno) << std::endl;
		return ;
	}
	pid[0] = fork();
	if (pid[0] == 0)
	{
		this->cgiCommunication(client, request, config, pipe_to_CGI, pipe_from_CGI);
	}
	else
	{
		char	**env = cgiGenEnvVar(request, cookies);
		this->cgiExecute(request, config, env, pipe_to_CGI, pipe_from_CGI);
	}
	//waitpid();
}

void	cgi::cgiCommunication(Client &client, Request &request, Config &config, int *pipe_to_CGI, int *pipe_from_CGI)
{
	int					original_standard_fds[2];
	int					bytes_sent;
	size_t				total_count;
	const std::string	&request_str = request.getRawBody();
	size_t				mssg_len = request_str.size();
	std::vector<char>	buffer(config.buffer_size);
	int					bytes_read;

	close(pipe_to_CGI[PIPE_READ_END]);
	close(pipe_from_CGI[PIPE_WRITE_END]);
	total_count = 0;
	while (total_count < mssg_len)
	{
		bytes_sent = write(pipe_to_CGI[PIPE_WRITE_END], request_str.c_str() + total_count, config.buffer_size);
		if (bytes_sent == -1)
		{
			std::cerr << "Failed to send body to CGI" << std::endl;
			_status = HTTP_INTERNAL_SERVER_ERROR;
			return ;
		}
		total_count += bytes_sent;
	}
	close(pipe_to_CGI[PIPE_WRITE_END]);
	total_count = 0;
	bytes_read = config.buffer_size;
	while (bytes_read == config.buffer_size)
	{
		bytes_read = read(pipe_from_CGI[PIPE_READ_END], &buffer[0] , buffer.size());
		if (bytes_sent == -1)
		{
			std::cerr << "Failed to send body to CGI" << std::endl;
			_status = HTTP_INTERNAL_SERVER_ERROR;
			return ;
		}
		if (client.socketWrite(&buffer[0], buffer.size()) == SERV_ERROR)
			return ;
		total_count += bytes_sent;
	}
	this->cgiRestoreFds(original_standard_fds);
}

void	cgi::cgiExecute(Request &request, Config &config, char	**env, int *pipe_to_CGI, int *pipe_from_CGI)
{
	int		original_standards_fds[2];

	close(pipe_to_CGI[PIPE_WRITE_END]);
	close(pipe_from_CGI[PIPE_READ_END]);
	if (dup2(pipe_to_CGI[PIPE_READ_END], STDIN_FILENO) == -1
		|| dup2(pipe_from_CGI[PIPE_WRITE_END], STDOUT_FILENO) == -1)
	{
		std::cerr << "dup2 initialisation failed" << std::endl;
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
	}
	close(pipe_to_CGI[PIPE_READ_END]);
	close(pipe_from_CGI[PIPE_WRITE_END]);
	
	(void)request;
	(void)config;
	
	cgiRestoreFds(original_standards_fds);
}

std::string	&cgi::cgiPassOutput()
{
	std::string *a = new std::string("lala");
	return (*a);
}

void		cgi::cgiReadInput(std::string &input)
{
	(void)input;
}

void		cgi::cgiRestoreFds(int *original_standard_fds)
{
	if (dup2(original_standard_fds[STDOUT_FILENO], STDOUT_FILENO) == -1
		|| dup2(original_standard_fds[STDIN_FILENO], STDIN_FILENO) == -1)
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
}

char	**cgi::cgiGenEnvVar(Request &request, std::vector<Cookie *> cookies)
{
	std::vector<std::string>	env;
	std::string					varvalue;
	char						**ret;
	
	env.push_back("METHOD=" + utils::methodToStr(request.getMethod()));
	env.push_back("QUERY_STRING=" + request.getQueryString());
	for (unsigned int i = 0; i < cookies.size(); ++i)
	{
		varvalue = cookies[i]->getSessionUID();
		env.push_back("HTTP_COOKIE=" + varvalue.substr(0, varvalue.find("=")));
		env.push_back("COOKIE_ID_VALUE=" + varvalue.erase(0, varvalue.find("=") + 1));
		env.push_back("COOKIE_EXP_TIME=" + utils::toString(cookies[i]->getExpirationTime()));
		std::map<std::string, std::string>	attr = cookies[i];
	}

	ret = new char *[env.size() + 1];
	for (unsigned int i = 0; i < env.size(); ++i)
		ret[i] = &env[i][0];
	ret[env.size()] = NULL;
	return (ret);
}
