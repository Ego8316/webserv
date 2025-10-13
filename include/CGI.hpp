/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:50 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/13 16:11:45 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Request.hpp"
#include "utils.hpp"

class CGI {
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
		void		CGIRun(Request &request);
		void		CGICommunication(int *pid, int *pipeToCGI, int *pipeFromCGI);
		void		CGIExecute(Request &request, int *pipeToCGI, int *pipeFromCGI);
		std::string	&CGIPassOutput();
		void		CGIReadInput(std::string &input);
	private :
		Request		_request;
		HttpStatus	_status;
};