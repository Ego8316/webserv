/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:46 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/24 23:42:42 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

/**
 * @brief Default constructor initializing CGI state.
 */
CGI::CGI()
{
	this->_cgi_script = "";
	this->_is_init = false;
	this->_is_complete = false;
	this->_status = HTTP_OK;
	this->_output = "";
	this->_header_len = 0;
	this->_content_len = 0;
	this->_pid = 0;
	this->_process_status[0] = 0;
	this->_process_status[1] = 0;
	this->_pipe_to_CGI[0] = 0;
	this->_pipe_to_CGI[1] = 0;
	this->_pipe_from_CGI[0] = 0;
	this->_pipe_from_CGI[1] = 0;
 	this->_total_bytes_sent = 0;
	this->_bytes_to_send = 0;
	this->_chunked = false;
	this->_cgi_script_char = NULL;
	this->_args = NULL;
	this->_env = NULL;
}

/**
 * @brief Copy constructor.
 *
 * @param other Source CGI.
 */
CGI::CGI(const CGI &other)
{
	*this = other;
}

/**
 * @brief Assignment operator.
 *
 * @param other Source CGI.
 *
 * @return Reference to this CGI.
 */
CGI &CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		this->_is_init = other._is_complete;
		this->_is_complete = other._is_complete;
		this->_status = other._status;
		this->_output = other._output;
		this->_header_len = other._header_len;
		this->_content_len = other._content_len;
		this->_pid = other._pid;
		this->_process_status[0] = other._process_status[0];
		this->_process_status[1] = other._process_status[1];
		this->_pipe_to_CGI[0] = other._pipe_to_CGI[0];
		this->_pipe_to_CGI[1] = other._pipe_to_CGI[1];
		this->_pipe_from_CGI[0] = other._pipe_from_CGI[0];
		this->_pipe_from_CGI[1] = other._pipe_from_CGI[1];
		this->_total_bytes_sent = other._total_bytes_sent;
		this->_bytes_to_send = other._bytes_to_send;
		this->_chunked = other._chunked;
		this->_cgi_script_char = NULL;
		this->_args = NULL;
		this->_env = NULL;
	}
	return (*this);
}

/**
 * @brief Destructor terminating process and cleaning pipes/env.
 */
CGI::~CGI()
{
	if (this->_process_status[0] == 0 && this->_pid != 0)
		kill(this->_pid, SIGKILL);
	if (this->_pipe_to_CGI[PIPE_WRITE_END] != -1)
	{
		close(this->_pipe_to_CGI[PIPE_WRITE_END]);
		this->_pipe_to_CGI[PIPE_WRITE_END] = -1;
	}
	if (this->_pipe_from_CGI[PIPE_READ_END] != -1)
	{
		close(this->_pipe_from_CGI[PIPE_READ_END]);
		this->_pipe_from_CGI[PIPE_READ_END] = -1;
	}
	deleteEnvVar();
}


/**
 * @brief Runs the CGI lifecycle (init then nanny loop).
 *
 * @param client Client issuing the request.
 * @param request Parsed HTTP request.
 * @param config Server configuration.
 * @param response Response to populate.
 */
void		CGI::Run(Client &client, Request &request, const ServerConfig &config, Response &response)
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
	this->_cgi_script = request.getRequestTarget(); //TODO quand config sera ok refaire les setup de path et les checks
	if (!utils::startsWith(this->_cgi_script, "."))
		this->_cgi_script = "." + this->_cgi_script;
	this->_cgi_script_char = new char[this->_cgi_script.length() + 1];
	strcpy(this->_cgi_script_char, this->_cgi_script.c_str());
	GenEnvVar(request);
	this->_pid = fork();
	if (this->_pid == 0)
	{
		if (chdir(config.root.c_str()) == -1)
			exit (1);
		this->Execute();
	}
	else
	{
		close(this->_pipe_to_CGI[PIPE_READ_END]);
		close(this->_pipe_from_CGI[PIPE_WRITE_END]);
		ServerCore::setNonBlocking(this->_pipe_to_CGI[PIPE_WRITE_END]);
		ServerCore::setNonBlocking(this->_pipe_from_CGI[PIPE_READ_END]);
		this->Nanny(client, request, config, response);
	}
	this->_is_init = true;
}

/**
 * @brief Supervises CGI I/O once initialized.
 *
 * @param client Client issuing the request.
 * @param request Parsed request.
 * @param config Server configuration.
 * @param response Response to populate when CGI completes.
 */
void	CGI::Nanny(Client &client, Request &request, const ServerConfig &config, Response &response)
{
	ssize_t				bytes_read = 1;
	ssize_t				bytes_sent = 1;
	
	if (this->_bytes_to_send == 0)
	{
		this->_bytes_to_send = request.getRawBody().size() + request.getRawHeader().size();
		std::cout << "Body = >" << request.getRawBody() << "<\nheader = >" << request.getRawHeader() << "<" << std::endl;
	}
	while (utils::getTime() < client.getTimeLimit() && !this->_is_complete)
	{
		if (this->_total_bytes_sent < this->_bytes_to_send || bytes_sent == 0)
			bytes_sent = this->writeToCGI(request, config);
		if (!checkOutputTermination(bytes_read))
			bytes_read = this->readFromCGI(config);
		else if (this->_process_status[0] == 0) // TODO what if the prorgram crash ???
			this->_process_status[0] = waitpid(this->_pid, &(this->_process_status[1]), WNOHANG);
		if (this->_process_status[0] != 0) // only set process status if done reading so we are sure the program is both finished and we are done reading
		{
			genFullOutput(response);
			this->_is_complete = true;
			std::cout << "this->_chunked = " << this->_chunked << " this->_content_len = " << this->_content_len << " this->_header_len = " << this->_header_len << " bytes_read = " << bytes_read << std::endl;
			std::cout << "this->_total_bytes_sent " << this->_total_bytes_sent << "this->_bytes_to_send " << this->_bytes_to_send << "bytes_sent " << bytes_sent << "checkOutputTermination(bytes_read) " << checkOutputTermination(bytes_read) << std::endl;
		}
	}
}

/**
 * @brief Writes request data to the CGI process.
 *
 * @param request Parsed request.
 * @param config Server configuration (buffer size).
 * @return Bytes written or -1 on error.
 */
ssize_t		CGI::writeToCGI(Request &request, const ServerConfig &config)
{
	ssize_t				bytes_sent = 0;
	std::vector<char>	buffer(config.client_body_buffer_size);
	int 				bts;
	
	if (this->_total_bytes_sent < static_cast<ssize_t>(request.getRawHeader().size()))
	{
		const std::string	&header_str = request.getRawHeader();
		bts = std::min(config.client_body_buffer_size, header_str.size() - this->_total_bytes_sent);
		bytes_sent = write(this->_pipe_to_CGI[PIPE_WRITE_END], header_str.c_str() + this->_total_bytes_sent, bts);
	}
	else
	{
		const std::string	&request_str = request.getRawBody();
		bts = std::min(config.client_body_buffer_size, static_cast<size_t>(this->_bytes_to_send - this->_total_bytes_sent));
		bytes_sent = write(this->_pipe_to_CGI[PIPE_WRITE_END], request_str.c_str() + this->_total_bytes_sent - request.getRawHeader().size(), bts);
	}
	if (bytes_sent != -1)
		this->_total_bytes_sent += bytes_sent;
	return (bytes_sent);
}

/**
 * @brief Reads output from the CGI process.
 *
 * @param config Server configuration (buffer size).
 * @return Bytes read or -1 on error.
 */
ssize_t		CGI::readFromCGI(const ServerConfig &config)
{
	ssize_t				bytes_read = 0;
	std::vector<char>	buffer(config.client_body_buffer_size + 1);

	bytes_read = read(this->_pipe_from_CGI[PIPE_READ_END], &buffer[0], config.client_body_buffer_size);
	if (bytes_read != -1)
	{
		std::cout << "read " << bytes_read << " now at " << this->_total_bytes_read << "/" <<  this->_output.length() << std::endl;
		buffer[bytes_read] = '\0';
		this->_output += std::string(buffer.begin(), buffer.begin() + bytes_read);
		this->_total_bytes_read += bytes_read;
	}
	if (!this->_header_len)
		this->parseHeader();
	return (bytes_read);
}

/**
 * @brief Parses CGI output header to fill metadata.
 */
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

/**
 * @brief Builds the final HTTP response from CGI output.
 *
 * @param response Response to populate.
 */
void	CGI::genFullOutput(Response &response)
{
	if (!WIFEXITED(this->_process_status[1]))
	{
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
		std::cout << "Error : Child process returned " << this->_process_status[1] << std::endl;
	}
	else if (utils::startsWith(this->_output, "HTTP/"))
		return ;
	else if (utils::caseInsensitiveFind(this->_output, "\r\nstatus: ") != this->_output.end())
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
	if (!this->_content_len && !this->_chunked)
		response.setContentLength(this->_output.length() - this->_output.find("\r\n\r\n"));
	response.buildHeader();
	response.setBody(this->_output);
	response.build();
	return ;
}

/**
 * @brief Returns the raw CGI output buffer.
 *
 * @return Reference to output string.
 */
std::string	&CGI::getOutput()
{
	return (this->_output);
}

/**
 * @brief Returns the CGI-generated status code.
 *
 * @return HttpStatus value.
 */
HttpStatus	CGI::getStatus()
{
	return (this->_status);
}

/**
 * @brief Indicates whether CGI processing finished.
 *
 * @return True when complete.
 */
bool		CGI::isComplete()
{
	return (this->_is_complete);
}

/**
 * @brief Forked child entry to exec the CGI script.
 */
void		CGI::Execute()
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
	if (execve(this->_cgi_script_char, this->_args, this->_env) == -1)
	{
		std::cerr << RED << "CGI execution failed" << RESET << std::endl;
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
		exit(1);
	}
}

/**
 * @brief Builds the CGI environment variables and argv.
 *
 * @param request Parsed HTTP request.
 */
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
			deleteEnvVar();
			return ;
		}
		strcpy(this->_env[i], env[i].c_str());
	}
	this->_env[env.size()] = NULL;
	this->_args = new char *[2];
	this->_args[0] = new char[this->_cgi_script.size() + 1];
	strcpy(this->_args[0], this->_cgi_script_char);
	this->_args[1] = NULL;
	return ;
}

/**
 * @brief Frees allocated argv/env structures and script path buffer.
 */
void	CGI::deleteEnvVar()
{
	int i = 0;
	
	delete this->_cgi_script_char;
	this->_cgi_script_char = NULL;
	if (this->_args)
	{
		while (this->_args[i])
		{	
			delete[] this->_args[i];
			this->_args[i] = NULL;
			++i;
		}
		delete[] this->_args;
		this->_args = NULL;
	}
	i = 0;
	if (this->_env)
	{
		while (this->_env[i])
		{	
			delete[] this->_env[i];
			this->_env[i] = NULL;
			++i;
		}
		delete[] this->_env;
		this->_env = NULL;
	}
}

/**
 * @brief Determines whether CGI output is complete based on headers/encoding.
 *
 * @param bytes_read Last read byte count.
 *
 * @return True when no more data is expected.
 */
bool	CGI::checkOutputTermination(int bytes_read)
{
	if (this->_chunked)
		return (utils::endsWith(this->_output, NULL_CHUNK));
	else if (this->_content_len && this->_header_len)
		return (this->_output.size() >= this->_header_len + this->_content_len);
	else
		return (bytes_read == 0);
}
