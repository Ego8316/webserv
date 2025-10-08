/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:03 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/08 19:13:37 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "utils.hpp"
#include "Config.hpp"

class Client;

class Cookie {
	public :
	// CONSTRUCTORS
		//Cookie();
		Cookie(Config *config);
		Cookie(const Cookie &other);
		Cookie &operator=(const Cookie &other);
	//DESTUCTORS
		~Cookie();
	//GETTERS
		std::map<std::string, std::string>	const	&getAllAttributes() const;
		std::string			getSessionUID() const;
		Cookie				*getSessionByUID(std::map<std::string, Cookie *> &sessions, std::string uid);
		std::string const	getAttribute(std::string key) const;
		void				writeAttribute(std::string key, std::string newvalue);
		
	//SETTERS
		void				setSessionUID(std::string uid);
	//MEMBER FUNCTIONS
		Cookie				*getSession(std::map<std::string, Cookie *> &sessions, std::map<std::string, std::string> header);
		Cookie				*createSession(std::map<std::string, Cookie *> &sessions, std::map<std::string, std::string> header);
		int					updateAttribute(std::string field_name, std::string field_value);
		bool				hasAttribute(std::string key) const;
		int					removeSession(std::map<std::string, Cookie *> &sessions, std::string uid);
		bool				sessionExists(std::map<std::string, Cookie *> &sessions, std::string uid);
		void				removeExpired(std::map<std::string, Cookie *> &sessions);
		bool				isExpired() const;
		int					updateCookie(std::map<std::string, std::string> header);
		std::string			genHeader();
		int					getTime() const;
	//VARIABLES
	private :
		std::string									_session_uid;
		time_t										_life_time;
		std::map<std::string, std::string>			_attributes;
		bool										_http_only;
		bool										_secure;
		int											_generation_time;
		std::string									_path; //TODO
		std::string									_domain; //TODO
		static Config								*_config;
};

std::ostream	&operator<<(std::ostream &os, const Cookie &src);
