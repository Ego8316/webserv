/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:36:39 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/12 21:46:06 by victorviter      ###   ########.fr       */
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
		std::map<std::string, Redirection>		getRedirections() const;
		ParseError		getParseError();
		void			setIP(std::string ip_str);
		void			parseDefaultErrorPages(std::istringstream &conf_stream);
		void			parseMethod(std::istringstream &conf_stream);
		void			parseHttpRedir(std::istringstream &conf_stream);
		bool			isAcceptedMethod(Method method) const;
	//SETTERS
	//MEMBER FUNCTIONS
		unsigned int 							ip;
		int										port_number;
		std::string								host_name; //TODO
		int										domain;
		int										type;
		int										protocol;
		size_t									max_body_size;
		int										client_limit;
		int										incoming_queue_backlog;
		int										buffer_size;
		int										cookie_sessions_max;
		int										cookie_life_time;
		std::string								server_home;
		bool									enable_listdir;
		std::string								default_page;
		
		std::map<int, std::string>				default_error_pages;
		std::vector<Method>						accepted_methods;
		std::map<std::string, Redirection>		http_redir; //TODO finish this shit

		ParseError								parse_error;
	private :
};

std::ostream	&operator<<(std::ostream &os, const Config &src);
