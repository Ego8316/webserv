/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:03 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 21:43:18 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "utils.hpp"

class Client;

class Cookie {
	public :
	// CONSTRUCTORS
		Cookie();
		Cookie(const Cookie &other);
		Cookie &operator=(const Cookie &other);
	//DESTUCTORS
		~Cookie();
	//GETTERS
		std::map<std::string, std::string>	const	&getAllAttributes() const;
		int											getSessionId() const;
		std::string const							getAttribute(std::string key) const;
		static Cookie								*getSessionById(unsigned int idx);
	//SETTERS
		void				setSessionId(unsigned int id);
		void				writeAttribute(std::string key, std::string newvalue);
		void				appendAttribute(std::string key, std::string newvalue);
	//MEMBER FUNCTIONS
		//static int			initCookies();
		static Cookie		*getCookie(std::map<std::string, std::string> header);
		static int			findSession(std::map<std::string, std::string> header);
		static bool			sessionExists(int id);
		static int			createSession();
		static int			removeSession(int id);
		int					updateCookie(std::map<std::string, std::string> header);
		int					updateAttribute(std::string field_name, std::string field_value);
		bool				isExpired() const;
		bool				hasAttribute(std::string key) const;
		std::string			genHeader();
		int					getTime() const;
	//VARIABLES
	private :
		static const int						_max_age = 3600;
		static Cookie							*_sessions[MAX_COOKIE_SESSIONS];
		unsigned int							_session_id;
		time_t									_life_time;
		std::map<std::string, std::string>		_attributes;
		bool									_http_only;
		int										_generation_time;
};


std::ostream	&operator<<(std::ostream &os, const Cookie &src);