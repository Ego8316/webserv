/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:03 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/29 18:51:10 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Client.hpp"

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
		Theme		getTheme();
	//SETTERS
		void		setTheme(Theme theme);
	//MEMBER FUNCTIONS
		int			parseHeader(std::map<std::string, std::string> header);
	private :
		Theme							_theme;
		static std::vector<Client>		_client;
};
