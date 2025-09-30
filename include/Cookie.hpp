/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:03 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 14:32:24 by victorviter      ###   ########.fr       */
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
		Theme				getTheme();
	//SETTERS
		void				setTheme(Theme theme);
		void				setSessionID(unsigned int id);
	//MEMBER FUNCTIONS
		static int			initCookies();
		static Cookie		*getCookie(std::map<std::string, std::string> header);
		static int			findSession(std::map<std::string, std::string> header);
		static bool			sessionExists(int id);
		static int			createSession();
		static int			removeSession(int id);
		int					updateCookie(std::map<std::string, std::string> header);
		int					updateField(std::string field_name, std::string field_value);
		int					isExpired();
		bool				hasField(std::string key);
		std::string			getField(std::string key);
		void				writeField(std::string key, std::string newvalue);
		void				appendField(std::string key, std::string newvalue);
	//VARIABLES
	private :
		static std::vector<Cookie *>			_sessions;
		unsigned int							_session_id;
		time_t									_life_time;
		std::map<std::string, std::string>		_cookies;
};
