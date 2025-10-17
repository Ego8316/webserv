/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:03 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/16 16:26:20 by ego              ###   ########.fr       */
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
		static Cookie		*getSessionByUID(std::map<std::string, Cookie *> *sessions, std::string uid);
		std::string const	getAttribute(std::string key) const;
		void				writeAttribute(std::string key, std::string newvalue);
		
	//SETTERS
		void				setSessionUID(std::string uid);
	//MEMBER FUNCTIONS
		static Cookie		*getSession(std::map<std::string, Cookie *> *sessions, std::string header);
		static Cookie		*createSession(std::map<std::string, Cookie *> *sessions);
		static int			removeSession(std::map<std::string, Cookie *> *sessions, std::string uid);
		static bool			sessionExists(std::map<std::string, Cookie *> *sessions, std::string uid);
		static void			removeExpired(std::map<std::string, Cookie *> *sessions);
		static int			getTime();
		int					updateAttribute(std::string field_name, std::string field_value);
		bool				hasAttribute(std::string key) const;
		bool				isExpired() const;
		int					updateCookie(std::string header);
		std::string			genHeader();
		bool				applyToPath(std::string path);
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
