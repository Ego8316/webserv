/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:50 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/13 20:04:02 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Request.hpp"
#include "utils.hpp"

class cgi {
	public :
	// CONSTRUCTORS
		cgi();
		cgi(const cgi &other);
		cgi &operator=(const cgi &other);
	//DESTUCTORS
		~cgi();
	//GETTERS
	//SETTERS
	//MEMBER FUNCTIONS
		void		cgiRun(Client &client, Request &request, Config &config);
		void		cgiCommunication(Client &client, Request &request, Config &config, int *pipeToCGI, int *pipeFromCGI);
		void		cgiExecute(Request &request, Config &config, int *pipeToCGI, int *pipeFromCGI);
		std::string	&cgiPassOutput();
		void		cgiReadInput(std::string &input);
	private :
		HttpStatus	_status;
};