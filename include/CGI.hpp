/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:50 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/28 11:03:33 by victorviter      ###   ########.fr       */
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

class	Cookie;
class	Client;
class	Request;
class	Response;
class	ServerCore;
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
		void		Run(Client &client, Request &request, const Config &config, Response &response, ServerCore &_server);
		
		void		Nanny(Client &client, Request &request, const Config &config, Response &response, ServerCore &server);
		ssize_t		writeToCGI(Request &request, const Config &config, ServerCore &server);
		ssize_t		readFromCGI(const Config &config, ServerCore &server);
		
		void		parseHeader(const Config &config);
		void		genFullOutput(Response &response);

		void		Execute();
		
		std::string	&getOutput();
		HttpStatus	getStatus();
		bool		isComplete();
		
		bool		checkOutputTermination(int bytes_read);
		void		GenEnvVar(Request &request);
		void		deleteEnvVar();
		
		int			*getPipesToCGI();
		int			*getPipesFromCGI();
		bool		getPipesPolled();
		void		setPipesPolled(bool value);
		void		setClientId(int value);
	private :
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
		bool				_pipes_polled;
};
