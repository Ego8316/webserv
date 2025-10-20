/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:50 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/20 23:56:56 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Request.hpp"
#include "utils.hpp"

class CGI
{
	public :
	// CONSTRUCTORS
		CGI();
		CGI(const CGI &other);
		CGI &operator=(const CGI &other);
	//DESTUCTORS
		~CGI();
	//GETTERS
	//SETTERS
	//MEMBER FUNCTIONS
		void		Run(Client &client, Request &request, Config &config, Cookie *cookies);
		
		void		Communicate(Client &client, Request &request, Config &config);
		void		writeToCGI(Request &request, Config &config);
		void		readFromCGI(Config &config);
		std::string	&getOutput();
		
		void		parseHeader();
		void		getFullHeader();

		void		Execute(Request &request, char **env);
		
		void		RestoreFds(int *original_standard_fds);
		char		**GenEnvVar(Request &request, Cookie *cookies);
	private :
		HttpStatus	_status;
		std::string	_output;
		std::string	_header;
		bool		_header_sent;
		int			_header_len;
		int			_content_len;
		int			_pid;
		int			_process_status[2];
		int			_pipe_to_CGI[2];
		int			_pipe_from_CGI[2];
		ssize_t		_total_bytes_sent;
		size_t		_bytes_to_send;
		ssize_t		_total_bytes_read;
		size_t		_total_bytes_to_read;
		bool		_chunked;
};
