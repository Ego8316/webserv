/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:09 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/14 12:36:07 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

Cookie::Cookie(std::string header)
{
	this->updateCookie(header);
	return ;
}

Cookie::Cookie(const Cookie &other)
{
	*this = other;
}

Cookie &Cookie::operator=(const Cookie &other)
{
	if (this != &other)
	{
		this->_session_uid = other._session_uid;
		this->_attributes = other.getAllAttributes();
	}
	return (*this);
}

Cookie::~Cookie() {}

bool		Cookie::hasAttribute(std::string key) const
{
	if (this->_attributes.find(key) == this->_attributes.end())
		return (false);
	return (true);
}

void		Cookie::writeAttribute(std::string key, std::string newvalue)
{
	if (newvalue == "")
		newvalue = "TRUE";
	if (this->getAttribute(key) == newvalue)
		return ;
	this->_attributes[key] = newvalue;
}

std::string const	Cookie::getAttribute(std::string key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_attributes.find(key);
    if (it == _attributes.end())
		return ("");
    return it->second;
}

std::map<std::string, std::string> const	&Cookie::getAllAttributes() const
{
	return (this->_attributes);
}

void		Cookie::setSessionUID(std::string uid)
{
	this->_session_uid = uid;
}

std::string			Cookie::getSessionUID() const
{
	return (this->_session_uid);
}

int			Cookie::updateCookie(std::string header)
{
	std::vector<std::string>	field_split = utils::stringSplit(header, "; ");
	std::istringstream			line(header);
	std::string					field_name;
	std::string					field_value;
	size_t						pos;
	std::string					separator;

	for (unsigned int i = 0; i < field_split.size(); ++i)
	{
		pos = field_split[i].find("=");
		field_name = field_split[i].substr(0, pos);
		field_value = field_split[i].erase(0, pos + 1);
		if (field_name == "Path")
			this->_path = field_value;
		else
			writeAttribute(field_name, field_value);
	}
	return (0);
}

bool		Cookie::applyToPath(std::string path)
{
	if (this->_path.length() == 0 || this->_path == "/")
		return (true);
	if (utils::startsWith(path, _path))
		return (true);
	return (false);
}

std::ostream	&operator<<(std::ostream &os, const Cookie &item)
{
	std::map<std::string, std::string>				attr = item.getAllAttributes();
	std::map<std::string, std::string>::iterator	it;
	os << "Cookie :" << std::endl;
	os << "session id: " << item.getSessionUID() << std::endl;
	os << "attributes: " << std::endl;
	for (it = attr.begin(); it != attr.end(); ++it)
		os << it->first << ": " << it->second << std::endl;
	os << "End attributes" << std::endl;
	return (os);
}
