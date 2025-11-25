/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:03 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/24 23:40:41 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

class Client;

/**
 * @class Cookie
 *
 * @brief Parses and stores HTTP cookie attributes from request headers.
 */
class Cookie {
	public :
	// CONSTRUCTORS
		Cookie();
		Cookie(std::string header);
		Cookie(const Cookie &other);
		Cookie &operator=(const Cookie &other);
	//DESTUCTORS
		~Cookie();
	//GETTERS
		std::map<std::string, std::string>	const	&getAllAttributes() const;
		std::string const							getAttribute(std::string key) const;
		std::string const							getPath() const;
		
	//SETTERS
		void				setAttribute(std::string key, std::string newvalue);
	//MEMBER FUNCTIONS
		int					updateAttribute(std::string field_name, std::string field_value);
		bool				hasAttribute(std::string key) const;
		int					updateCookie(std::string header);
		bool				applyToPath(std::string path);
	//VARIABLES
	private :
		std::map<std::string, std::string>			_attributes;
		std::string									_path;
};

std::ostream	&operator<<(std::ostream &os, const Cookie &src);
