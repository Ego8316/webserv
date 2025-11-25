/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:09 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/24 23:46:55 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

/**
 * @brief Default-constructs a cookie with path set to root.
 */
Cookie::Cookie()
{
	this->_path = "/";
	return ;
}

/**
 * @brief Builds a cookie by parsing a raw `Cookie:` header value.
 *
 * @param header Raw Cookie header string.
 */
Cookie::Cookie(std::string header)
{
	this->updateCookie(header);
	return ;
}

/**
 * @brief Copy constructor.
 *
 * @param other Source cookie.
 */
Cookie::Cookie(const Cookie &other)
{
	*this = other;
}

/**
 * @brief Assignment operator.
 *
 * @param other Source cookie.
 *
 * @return Reference to this cookie.
 */
Cookie &Cookie::operator=(const Cookie &other)
{
	if (this != &other)
	{
		this->_path = other._path;
		this->_attributes = other.getAllAttributes();
	}
	return (*this);
}

/**
 * @brief Destructor.
 */
Cookie::~Cookie() {}

/**
 * @brief Checks if an attribute is present.
 *
 * @param key Attribute name.
 *
 * @return True when found, false otherwise.
 */
bool		Cookie::hasAttribute(std::string key) const
{
	if (this->_attributes.find(key) == this->_attributes.end())
		return (false);
	return (true);
}

/**
 * @brief Sets or overwrites an attribute, defaulting empty values to "TRUE".
 *
 * @param key Attribute name.
 * @param newvalue Attribute value (falls back to "TRUE" when empty).
 */
void		Cookie::setAttribute(std::string key, std::string newvalue)
{
	if (newvalue == "")
		newvalue = "TRUE";
	if (this->getAttribute(key) == newvalue)
		return ;
	this->_attributes[key] = newvalue;
}

/**
 * @brief Returns the value of a named attribute or an empty string.
 *
 * @param key Attribute name.
 * @return Attribute value or empty string.
 */
const std::string	Cookie::getAttribute(std::string key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_attributes.find(key);
    if (it == _attributes.end())
		return ("");
    return it->second;
}

/**
 * @brief Returns all attributes.
 *
 * @return Const reference to the attribute map.
 */
const std::map<std::string, std::string>	&Cookie::getAllAttributes() const
{
	return (this->_attributes);
}

/**
 * @brief Parses and updates attributes from a raw `Cookie:` header value.
 *
 * @param header Raw Cookie header string.
 * 
 * @return 0 on success.
 */
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
		setAttribute(field_name, field_value);
	}
	return (0);
}

/**
 * @brief Checks if the cookie path matches a requested resource path.
 *
 * @param path Requested URI path.
 *
 * @return True when the cookie applies to the path.
 */
bool		Cookie::applyToPath(std::string path)
{
	if (this->_path.length() == 0 || this->_path == "/")
		return (true);
	if (utils::startsWith(path, _path))
		return (true);
	return (false);
}

/**
 * @brief Returns the cookie path attribute.
 *
 * @return Cookie path.
 */
const std::string	Cookie::getPath() const
{
	return (this->_path);
}

/**
 * @brief Stream insertion for debugging cookie contents.
 *
 * @param os Output stream.
 * @param item Cookie to render.
 *
 * @return Reference to the output stream.
 */
std::ostream	&operator<<(std::ostream &os, const Cookie &item)
{
	std::map<std::string, std::string>				attr = item.getAllAttributes();
	std::map<std::string, std::string>::iterator	it;
	os << "Cookie :" << std::endl;
	os << "Path = " << item.getPath();
	os << "attributes: " << std::endl;
	for (it = attr.begin(); it != attr.end(); ++it)
		os << it->first << ": " << it->second << std::endl;
	os << "End attributes" << std::endl;
	return (os);
}
