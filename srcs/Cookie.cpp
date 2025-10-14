/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:09 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/14 11:11:00 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

Config		*Cookie::_config;

Cookie::Cookie(Config *config)
{
	this->_config = config;
	return ;
}

Cookie::Cookie(const Cookie &other) : _session_uid(other._session_uid) {}

Cookie &Cookie::operator=(const Cookie &other)
{
	if (this != &other)
	{
		this->_session_uid = other._session_uid;
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

long										Cookie::getExpirationTime() const
{
	return (this->_expiration_time);
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
	if (_config->cookie_life_time == NO_TIMEOUT)
		this->_expiration_time = 0;
	else
		this->_expiration_time = this->getTime() + _config->cookie_life_time;
	return (0);
}

Cookie		*Cookie::getSession(Config *config, std::map<std::string, Cookie *> *sessions, std::string header)
{
	std::string		uid = "";
	Cookie			*found = NULL;
	
	std::vector<std::string>	cookie_in = utils::stringSplit(header, ";");
	
	if ((*sessions).find(cookie_in[0]) != (*sessions).end())
	{
		std::string					uid = cookie_in[0];
		std::vector<std::string>	cookie_in = utils::stringSplit(header, "=");

		if (!sessionExists(sessions, uid))
		{
			std::cerr << "Invalid session ID, could not retrieve cookies " << sessionExists(sessions, uid) << std::endl;
			found = NULL;
		}
		else
		{
			found = (*sessions)[uid];
			found->updateCookie(header);
			std::cout << "Found cookie session" << std::endl;
		}
	}
	if (found == NULL)
		found = createSession(config, sessions);
	return (found);
}

bool	Cookie::sessionExists(std::map<std::string, Cookie *> *sessions, std::string uid)
{
	if ((*sessions).find(uid) != (*sessions).end())
		return ((*sessions)[uid] != NULL);
	return (false);
}

Cookie		*Cookie::createSession(Config *config, std::map<std::string, Cookie *> *sessions)
{
	int 				uid = 0;
	std::string			uid_str;

	for (int i = 0; i < config->cookie_sessions_max; ++i)
	{
		if (!sessionExists(sessions, "session_uid=" + utils::toString(i)))
		{
			uid = i;
			break ;
		}
	}
	uid_str = "session_uid=" + utils::toString(uid);
	if (uid != config->cookie_sessions_max)
	{
		(*sessions)[uid_str] = new Cookie(config);
		(*sessions)[uid_str]->setSessionUID(uid_str);
		(*sessions)[uid_str]->_expiration_time = getTime() + config->cookie_life_time;
		std::cout << "created session " << uid_str << std::endl;
		return ((*sessions)[uid_str]);
	}
	std::cerr << "Cannot create new Cookie, limit reached" << std::endl;
	return (NULL);
}

void		Cookie::removeExpired(std::map<std::string, Cookie *> *sessions)
{
	std::map<std::string, Cookie *>::iterator	it = (*sessions).begin();
	while (it != (*sessions).end())
	{
		if (it->second != NULL && it->second->isExpired())
		{
			delete it->second;
			(*sessions)[it->first] = NULL;
			std::cout << "removed session " << it->first << std::endl;
			std::map<std::string, Cookie*>::iterator toErase = it++; //TODO check ok!
			sessions->erase(toErase);
		}
		else
			++it;
	}
	return ;
}

int		Cookie::removeSession(std::map<std::string, Cookie *> *sessions, std::string uid)
{
	if (sessionExists(sessions, uid))
	{
		delete (*sessions)[uid];
		(*sessions)[uid] = NULL;
		(*sessions).erase(uid);
		return (0);
	}
	return (SERV_ERROR);
}

Cookie			*Cookie::getSessionByUID(std::map<std::string, Cookie *> *sessions, std::string uid)
{
	if (sessionExists(sessions, uid))
		return ((*sessions)[uid]);
	return (NULL);
}

bool		Cookie::isExpired() const
{
	if (this->_config->cookie_life_time == NO_TIMEOUT)
		return (false);
	return (this->getTime() > this->_expiration_time);
}

int		Cookie::getTime()
{
	time_t		t;
	
	t = time(NULL);
	if (t == -1)
	{
		std::cerr << "Clock error in Cookie. Considering Cookie as expired" << std::endl;
		return (SERV_ERROR);
	}
	return (static_cast<int>(t));
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
