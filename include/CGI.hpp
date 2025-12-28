/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:50 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/28 22:58:21 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include "Cookie.hpp"
#include "utils.hpp"
#include "Response.hpp"
#include "ServerCore.hpp"
#include "ServerConfig.hpp"

class	Cookie;
class	Client;
class	Request;
class	Response;
class	ServerCore;

/**
 * @class CGI
 *
 * @brief Manages execution of CGI scripts and marshals I/O to/from responses.
 */
class CGI
{
	public :
		CGI(const std::string &cgi_script);
		~CGI();
		void		Run(Client &client, Request &request, const ServerConfig &config, Response &response, ServerCore &server);
		
		void		Nanny(Client &client, Request &request, const ServerConfig &config, Response &response, ServerCore &server);
		ssize_t		writeToCGI(Request &request, const ServerConfig &config, ServerCore &server);
		ssize_t		readFromCGI(const ServerConfig &config, ServerCore &server);
		
		void		parseHeader(const ServerConfig &config);
		void		genFullOutput(Response &response, const ServerConfig &config);

		void		Execute();
		
		void		setClientId(int value);
		std::string	&getOutput();
		HttpStatus	getStatus();
		bool		isComplete();
		
	private :
		void				_genEnvVar(Request &request);
		void				_deleteEnvVar();
		bool				_checkOutputTermination(int bytes_read);

		int					_client_id;
		std::string			_cgi_script;
		bool				_is_init;
		bool				_is_complete;
		HttpStatus			_status;
		std::string			_output;
		size_t				_header_len;
		size_t				_content_len;
		int					_pid;
		int					_process_status[2];
		int					_pipe_to_CGI[2];
		int					_pipe_from_CGI[2];
		ssize_t				_total_bytes_sent;
		ssize_t				_bytes_to_send;
		ssize_t				_total_bytes_read;
		bool				_chunked;
		char				*_cgi_script_char;
		char				**_args;
		char				**_env;
		int					_pipe_to_cgi_idx;
		int					_pipe_from_cgi_idx;
		ServerCore			*_server_link;
};
