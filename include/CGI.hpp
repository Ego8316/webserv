/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:08:50 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/19 17:19:18 by victorviter      ###   ########.fr       */
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
		void		Communication(Client &client, Request &request, Config &config, int *pipeToCGI, int *pipeFromCGI);
		void		Execute(Request &request, char **env, int *pipeToCGI, int *pipeFromCGI);
		void		RestoreFds(int *original_standard_fds);
		char		**GenEnvVar(Request &request, Cookie *cookies);
	private :
		HttpStatus	_status;
};