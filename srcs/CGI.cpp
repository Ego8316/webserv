/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:46 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/20 21:33:55 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI()
{
	this->_total_bytes_sent = 0;
	this->_bytes_to_send = 0;
	this->_total_bytes_to_read = 0;
	this->_output = "";
	this->_process_status[0] = 0;
	this->_process_status[1] = 0;
	this->_header_len = 0;
}

CGI::CGI(const CGI &other)
{
	*this = other;
}

CGI &CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		this->_status = other._status;
		this->_output = other._output;
		this->_pid = other._pid;
		this->_process_status[0] = other._process_status[0];
		this->_process_status[1] = other._process_status[1];
		this->_pipe_to_CGI[0] = other._pipe_to_CGI[0];
		this->_pipe_to_CGI[1] = other._pipe_to_CGI[1];
		this->_pipe_from_CGI[0] = other._pipe_from_CGI[0];
		this->_pipe_from_CGI[1] = other._pipe_from_CGI[1];
		this->_total_bytes_sent = other._total_bytes_sent;
		this->_bytes_to_send = other._bytes_to_send;
		this->_total_bytes_read = other._total_bytes_read;
		this->_total_bytes_to_read = other._total_bytes_to_read;
		this->_header_sent = other._header_sent;
		this->_header_len = other._header_len;
		this->_chunked = other._chunked;
	}
	return (*this);
}

CGI::~CGI() {}


void		CGI::Run(Client &client, Request &request, Config &config, Cookie *cookies)
{
	if (pipe(this->_pipe_to_CGI) == -1)
	{
		std::cerr << "Could not initialize pipe " << strerror(errno) << std::endl;
		return ;
	}
	if (pipe(this->_pipe_from_CGI) == -1)
	{
		std::cerr << "Could not initialize pipe " << strerror(errno) << std::endl;
		return ;
	}
	this->_pid = fork();
	if (this->_pid == 0)
	{
		char	**env = GenEnvVar(request, cookies);
		this->Execute(request, env);
	}
	else
	{
		this->Communicate(client, request, config);
	}
	//waitpid();
}

void	CGI::Communicate(Client &client, Request &request, Config &config)
{
	int					original_standard_fds[2];

	close(this->_pipe_to_CGI[PIPE_READ_END]);
	close(this->_pipe_from_CGI[PIPE_WRITE_END]);
	if (this->_bytes_to_send == 0)
		this->_bytes_to_send = request.getRawBody().size();
	while (utils::getTime() < client.getTimeLimit())
	{
		if (this->_total_bytes_sent < this->_bytes_to_send)
			this->writeToCGI(request, config);
		if (!this->_header_len || this->_total_bytes_read < this->_total_bytes_to_read)
			readFromCGI(config);
		else if (this->_process_status[0] == 0)
			this->_process_status[0] = waitpid(this->_pid, &(this->_process_status[1]), WNOHANG);
		if (this->_process_status[0] != 0)
		{
			this->sendOutput(client, config);
		}
	}
	//if (state == done);
	//	this->RestoreFds(original_standard_fds);
}

void	CGI::writeToCGI(Request &request, Config &config)
{
	ssize_t				bytes_sent = 0;
	const std::string	&request_str = request.getRawBody();
	std::vector<char>	buffer(config.buffer_size);

	bytes_sent = send(this->_pipe_to_CGI[PIPE_WRITE_END],
			request_str.c_str() + this->_total_bytes_sent, config.buffer_size, MSG_DONTWAIT);
	if (bytes_sent == -1)
	{
		std::cerr << "Failed to send body to CGI" << std::endl;
		_status = HTTP_INTERNAL_SERVER_ERROR;
		return ;
	}
	this->_total_bytes_sent += bytes_sent;
}

void	CGI::readFromCGI(Config &config)
{
	ssize_t				bytes_read = 0;
	std::vector<char>	buffer(config.buffer_size);

	bytes_read = recv(this->_pipe_from_CGI[PIPE_READ_END], &buffer[0] , buffer.size(), MSG_DONTWAIT);
	if (bytes_read == -1)
	{
		std::cerr << "Failed to send body to CGI" << std::endl;
		_status = HTTP_INTERNAL_SERVER_ERROR;
		return ;
	}
	this->_output += std::string(buffer.begin(), buffer.end() + bytes_read);
	this->_total_bytes_sent += bytes_read;
	if (!this->_header_len)
		this->parseHeader();
}

void	CGI::parseHeader()
{
	if (!this->_output.length())
		return ;
	if (utils::startsWith(this->_output, "HTTP/"))
		this->_header_sent = true;
	if (utils::caseInsensitiveFind(this->_output, "Content-Length: ") != this->_output.end())
	{
		this->_bytes_to_send = atoi(&*utils::caseInsensitiveFind(this->_output, "Content-Length: ")
			+ std::string("Content-Length: ").length());
	}
	else if (utils::caseInsensitiveFind(this->_output, "Transfer-Encoding: chunked") != this->_output.end())
		this->_chunked = true;
	if (this->_output.find("\r\n\r\n") != std::string::npos)
	{
		this->_header_len = true;
	}
}

void	CGI::getFullHeader()
{
	if (!WIFEXITED(this->_process_status[1]))
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
	else if (this->_header_sent)
		return ;
	else if (utils::caseInsensitiveFind(this->_output, "status: ") != this->_output.end())
	{
		this->_status = utils::strToHttpStatus(&*utils::caseInsensitiveFind(this->_output, "status: ")
			+ std::string("status: ").length());
		if (this->_status == HTTP_UNKNOWN_STATUS)
			this->_status = HTTP_INTERNAL_SERVER_ERROR;
		this->_output.erase(0, this->_output.find("\r\n"));
	}
	else
		this->_status = HTTP_OK;
	
}

void	CGI::sendOutput(Client &client, Config &config)
{
	std::vector<char>	buffer(config.buffer_size);

}

void	CGI::Execute(Request &request, char	**env)
{
	int		original_standard_fds[2];

	close(this->_pipe_to_CGI[PIPE_WRITE_END]);
	close(this->_pipe_from_CGI[PIPE_READ_END]);
	if (dup2(this->_pipe_to_CGI[PIPE_READ_END], STDIN_FILENO) == -1
		|| dup2(this->_pipe_from_CGI[PIPE_WRITE_END], STDOUT_FILENO) == -1)
	{
		std::cerr << "dup2 initialisation failed" << std::endl;
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
	}
	close(this->_pipe_to_CGI[PIPE_READ_END]);
	close(this->_pipe_from_CGI[PIPE_WRITE_END]);
	char	**argv = new char *[2];
	argv[0] = &request.getRequestTarget()[0];
	argv[1] = NULL;
	if (execve(request.getRequestTarget().c_str(), argv, env) == -1)
	{
		std::cerr << "CGI execution failed" << std::endl;
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
	}
	RestoreFds(original_standard_fds);
	delete[] env;
	delete env;
}

void		CGI::RestoreFds(int *original_standard_fds)
{
	if (dup2(original_standard_fds[STDOUT_FILENO], STDOUT_FILENO) == -1
		|| dup2(original_standard_fds[STDIN_FILENO], STDIN_FILENO) == -1)
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
}

char	**CGI::GenEnvVar(Request &request, Cookie *cookies)
{
	std::vector<std::string>	env;
	std::string					varvalue;
	char						**ret;
	
	env.push_back("METHOD=" + utils::methodToStr(request.getMethod()));
	env.push_back("QUERY_STRING=" + request.getQueryString());
	if (cookies != NULL)
	{
		std::map<std::string, std::string>	attr = cookies->getAllAttributes();
		for (std::map<std::string, std::string>::iterator it = attr.begin(); it != attr.end(); ++it)
		{
			env.push_back("HTTP_COOKIE_" + it->first + "=" + it->second);
		}
	}
	ret = new char *[env.size() + 1];
	for (unsigned int i = 0; i < env.size(); ++i)
	{
		ret[i] = new char[env[i].length() + 1];
		if (!ret[i])
		{	//TODO test
			delete[] ret;
			delete ret;
			return (NULL);
		}
		strcpy(ret[i], env[i].c_str());
	}
	ret[env.size()] = NULL;
	return (ret);
}
