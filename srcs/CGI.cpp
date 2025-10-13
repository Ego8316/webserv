/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:46 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/13 16:54:19 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI() {}

CGI::CGI(const CGI &other)
{
}

CGI &CGI::operator=(const CGI &other)
{
}

CGI::~CGI() {}


void		CGI::CGIRun(Request &request)
{
	int		pipeToCGI[2];
	int		pipeFromCGI[2];
	pid_t	pid[2];

	setenv("METHOD", utils::methodToStr(request.getMethod()).c_str(), 1);
	setenv("QUERY_STRING", request.getQueryString().c_str(), 1);

	
	if (pipe(pipeToCGI) == -1)
	{
		std::cerr << "Could not initialize pipe " << strerror(errno) << std::endl;
		return ;
	}
	if (pipe(pipeFromCGI) == -1)
	{
		std::cerr << "Could not initialize pipe " << strerror(errno) << std::endl;
		return ;
	}
	pid[0] = fork();
	if (pid[0] == 0)
	{
		this->CGICommunication(pid, pipeToCGI, pipeFromCGI);
	}
	else
	{
		this->CGIExecute(request, pipeToCGI, pipeFromCGI);
	}
	//waitpid();
}

void	CGI::CGICommunication(int *pid, int *pipeToCGI, int *pipeFromCGI)
{
	close(pipeToCGI[PIPE_READ_END]);
	close(pipeFromCGI[PIPE_WRITE_END]);
	if (dup2(pipeToCGI[PIPE_READ_END], STDIN_FILENO) == -1)
	{
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
		close(pipeToCGI[PIPE_WRITE_END]);
		close(pipeFromCGI[PIPE_READ_END]);
	}
	if (dup2(pipeToCGI[PIPE_READ_END], STDIN_FILENO) == -1)
	{
		this->_status = HTTP_INTERNAL_SERVER_ERROR;
		close(pipeToCGI[PIPE_WRITE_END]);
		close(pipeFromCGI[PIPE_READ_END]);
	}
}

void	CGI::CGIExecute(Request &request, int *pipeToCGI, int *pipeFromCGI)
{
	close(pipeToCGI[PIPE_WRITE_END]);
	close(pipeFromCGI[PIPE_READ_END]);
	
}

std::string	&CGI::CGIPassOutput()
{
	
}

void		CGI::CGIReadInput(std::string &input)
{
	
}


