/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:46 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/23 12:31:30 by victorviter      ###   ########.fr       */
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
	this->_is_complete = false;
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
		this->_header_len = other._header_len;
		this->_chunked = other._chunked;
	}
	return (*this);
}

CGI::~CGI()
{
	if (this->_process_status[0] == 0 && this->_pid != 0)
		kill(this->_pid, SIGKILL);
}


void		CGI::Run(Client &client, Request &request, Config &config, Response &response)
{
	if (this->_is_init)
		return (this->Nanny(client, request, config, response));
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
		GenEnvVar(request);
		this->Execute(request);
	}
	else
	{
		this->Nanny(client, request, config, response);
	}
	this->_is_init = true;
}

void	CGI::Nanny(Client &client, Request &request, Config &config, Response &response)
{
	ssize_t				bytes_read = 1;
	ssize_t				bytes_sent = 1;
	
	close(this->_pipe_to_CGI[PIPE_READ_END]);
	close(this->_pipe_from_CGI[PIPE_WRITE_END]);
	if (this->_bytes_to_send == 0)
		this->_bytes_to_send = request.getRawBody().size();
	while (utils::getTime() < client.getTimeLimit())
	{
		if (this->_total_bytes_sent < this->_bytes_to_send || bytes_sent == 0)
			bytes_sent = this->writeToCGI(request, config);
		if (!checkOutputTermination(bytes_read))
			bytes_read = this->readFromCGI(config);
		else if (this->_process_status[0] == 0)
			this->_process_status[0] = waitpid(this->_pid, &(this->_process_status[1]), WNOHANG);
		if (this->_process_status[0] != 0) // only set process status if done reading so we are sure the program is both finished and we are done reading
		{
			genFullOutput(response);
			this->_is_complete = true;
		}
	}
}

ssize_t		CGI::writeToCGI(Request &request, Config &config)
{
	ssize_t				bytes_sent = 0;
	const std::string	&request_str = request.getRawBody();
	std::vector<char>	buffer(config.buffer_size);

	bytes_sent = send(this->_pipe_to_CGI[PIPE_WRITE_END],
			request_str.c_str() + this->_total_bytes_sent, config.buffer_size, MSG_DONTWAIT);
	this->_total_bytes_sent += bytes_sent;
	return (bytes_sent);
}

ssize_t		CGI::readFromCGI(Config &config)
{
	ssize_t				bytes_read = 0;
	std::vector<char>	buffer(config.buffer_size);

	bytes_read = recv(this->_pipe_from_CGI[PIPE_READ_END], &buffer[0] , buffer.size(), MSG_DONTWAIT);
	this->_output += std::string(buffer.begin(), buffer.end() + bytes_read);
	this->_total_bytes_read += bytes_read;
	if (!this->_header_len)
		this->parseHeader();
	return (bytes_read);
}

void	CGI::parseHeader()
{
	if (!this->_output.length() || utils::startsWith(this->_output, "HTTP/"))
		return ;
	if (utils::caseInsensitiveFind(this->_output, "Content-Length: ") != this->_output.end())
	{
		this->_content_len = atoi(&*utils::caseInsensitiveFind(this->_output, "Content-Length: ")
			+ std::string("Content-Length: ").length());
	}
	else if (utils::caseInsensitiveFind(this->_output, "Transfer-Encoding: ") != this->_output.end())
	{
		size_t	line_start = utils::caseInsensitiveFind(this->_output, "Transfer-Encoding: ") - this->_output.begin();
		size_t	line_end = this->_output.find("\r\n", line_start);
		std::string		line = this->_output.substr(line_start, line_end);
		if (utils::caseInsensitiveFind(line, "chunked") != line.end())
			this->_chunked = true;
	}
	if (this->_output.find("\r\n\r\n") != std::string::npos)
		this->_header_len = this->_output.find("\r\n\r\n") + 4;
}

void	CGI::genFullOutput(Response &response)
{
	if (!WIFEXITED(this->_process_status[1]))
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
	else if (utils::startsWith(this->_output, "HTTP/"))
	{
		deleteEnvVar();
		return ;
	}
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
	response.setStatus(this->_status);
	//this->_header = "HTTP/1.0 " + utils::toString(this->_status)
	//	+ " " + utils::httpStatusToStr(this->_status) + "\r\n";
	//this->_header += "Server: Webserv/1.0 (Unix)\r\n";
	if (!this->_content_len && !this->_chunked)
		//this->_header += "Content-Length: " + utils::toString(this->_output.length() - this->_header_len) + "\r\n";
		response.setContentLength(this->_output.length() - this->_output.find("\r\n\r\n"));
	response.buildHeader();
	std::cout << "HEADER ====== \n" << response.getHeader() << std::endl;
	std::cout << "END HEADER ====== \n" << std::endl;
	deleteEnvVar();
	return ;
}

std::string	&CGI::getOutput()
{
	return (this->_output);
}

HttpStatus	CGI::getStatus()
{
	return (this->_status);
}

bool		CGI::isComplete()
{
	return (this->_is_complete);
}

void		CGI::Execute(Request &request)
{
	close(this->_pipe_to_CGI[PIPE_WRITE_END]);
	close(this->_pipe_from_CGI[PIPE_READ_END]);
	if (dup2(this->_pipe_to_CGI[PIPE_READ_END], STDIN_FILENO) == -1
		|| dup2(this->_pipe_from_CGI[PIPE_WRITE_END], STDOUT_FILENO) == -1)
	{
		std::cerr << "dup2 initialisation failed" << std::endl;
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
		exit(1);
	}
	if (execve(request.getRequestTarget().c_str(), this->_args, this->_env) == -1)
	{
		std::cerr << "CGI execution failed" << std::endl;
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
		exit(1);
	}
}

void		CGI::GenEnvVar(Request &request)
{
	std::vector<std::string>	env;
	std::string					varvalue;
	Cookie						cookies = request.getQueryCookies();
	
	env.push_back("METHOD=" + utils::methodToStr(request.getMethod()));
	env.push_back("QUERY_STRING=" + request.getQueryString());
	std::map<std::string, std::string>	attr = cookies.getAllAttributes();
	for (std::map<std::string, std::string>::iterator it = attr.begin(); it != attr.end(); ++it)
		env.push_back("HTTP_COOKIE_" + it->first + "=" + it->second);
	this->_env = new char *[env.size() + 1];
	for (unsigned int i = 0; i < env.size(); ++i)
	{
		this->_env[i] = new char[env[i].length() + 1];
		if (!this->_env[i])
		{	//TODO test
			delete[] this->_env;
			delete this->_env;
			return ;
		}
		strcpy(this->_env[i], env[i].c_str());
	}
	this->_env[env.size()] = NULL;
	this->_args = new char *[2];
	this->_args[0] = &request.getRequestTarget()[0];
	this->_args[1] = NULL;
	return ;
}

void	CGI::deleteEnvVar()
{
	int i = 0;

	while (this->_args[i])
		delete this->_args[i];
	delete[] this->_args;
	while (this->_env[i])
		delete this->_env[i];
	delete[] this->_env;
}

bool	CGI::checkOutputTermination(int bytes_read)
{
	if (this->_chunked)
		return (utils::endsWith(this->_output, NULL_CHUNK));
	else if (this->_content_len && this->_header_len)
		return (this->_output.size() >= this->_header_len + this->_content_len);
	else
		return (bytes_read == 0);
}
