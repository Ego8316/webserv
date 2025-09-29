/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:09 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/29 18:50:55 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

Cookie::Cookie() {}

Cookie::Cookie(const Cookie &other)
{
}

Cookie &Cookie::operator=(const Cookie &other)
{
}

Cookie::~Cookie() {}

Theme		Cookie::getTheme()
{
	return (this->_theme);
}

void		Cookie::setTheme(Theme theme)
{
	this->_theme = theme;
}

int		parseHeader(std::map<std::string, std::string> header)
{
	//TODO check if user exist and return the appropriate cookike;
	if (this)
	return (0);
}

