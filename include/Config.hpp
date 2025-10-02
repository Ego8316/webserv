/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:36:39 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/02 13:20:28 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

class Config {
	public :
	// CONSTRUCTORS
		//Config();
		Config(std::string filename);
		Config(const Config &other);
		Config &operator=(const Config &other);
	//DESTUCTORS
		~Config();
	//GETTERS
		ParseError		getParseError();
	//SETTERS
	//MEMBER FUNCTIONS
		int				port_number;
		int				time_out;
		int				client_limit;
		int				domain;
		int				type;
		int				protocol;
		int				buffer_size;
		std::string		ServHome;
		std::string		default_page;
		ParseError		parse_error;
		int				cookie_life_time;
		int				cookie_sessions_max;
		int				incomming_queue_backlog;
	private :
};

std::ostream	&operator<<(std::ostream &os, const Config &src);