/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:36:39 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/07 17:24:18 by victorviter      ###   ########.fr       */
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
		void			setIP(std::string ip_str);
		void			parseDefaultErrorPages(std::istringstream &conf_stream);
	//SETTERS
	//MEMBER FUNCTIONS
		unsigned int 					ip;
		int								port_number;
		int								client_limit;
		int								domain;
		int								type;
		int								protocol;
		int								buffer_size;
		std::string						server_home;
		std::string						default_page;
		std::map<int, std::string>		default_error_pages;
		ParseError						parse_error;
		int								cookie_life_time;
		int								cookie_sessions_max;
		int								incomming_queue_backlog;
		
		static std::vector<Config *>	parseMultipleConfigs(std::string filename);
		static void						deleteAllConfigs(std::vector<Config *> &configs);
	private :
};

std::ostream	&operator<<(std::ostream &os, const Config &src);