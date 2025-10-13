/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:09 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/13 12:05:45 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

Config		*Cookie::_config;

Cookie::Cookie(Config *config)
{
	this->_config = config;
	return ;
}

Cookie::Cookie(const Cookie &other) : _session_uid(other._session_uid), _life_time(other._life_time) {}

Cookie &Cookie::operator=(const Cookie &other)
{
	if (this != &other)
	{
		this->_session_uid = other._session_uid;
		this->_life_time = other._life_time;
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
		if (field_split[i].find("=") == std::string::npos)
		{
			if (field_split[i].find("HttpOnly"))
				this->_http_only = true;
			else if (field_split[i].find("Secure"))
				this->_secure = true;
		}
		pos = field_split[i].find("=");
		field_name = field_split[i].substr(0, pos);
		field_value = field_split[i].erase(0, pos + 1);
		if (field_name == "Path")
			this->_path = field_value;
		else if (field_name == "Domain")
			this->_domain = field_value;
		else
			writeAttribute(field_name, field_value);
	}
	this->_generation_time = this->getTime();
	if (this->_generation_time == SERV_ERROR)
		return (SERV_ERROR);
	return (0);
}

Cookie		*Cookie::getSession(std::map<std::string, Cookie *> *sessions, std::string header)
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
		found = createSession(sessions);
	return (found);
}

bool	Cookie::sessionExists(std::map<std::string, Cookie *> *sessions, std::string uid)
{
	if ((*sessions).find(uid) != (*sessions).end())
		return ((*sessions)[uid] != NULL);
	return (false);
}

Cookie		*Cookie::createSession(std::map<std::string, Cookie *> *sessions)
{
	int 				uid = 0;
	std::string			uid_str;
	std::ostringstream	convert;

	for (int uid = 0; uid < _config->cookie_sessions_max; ++uid)
	{
		convert.str("");
		convert << uid;
		if (!sessionExists(sessions, "session_uid=" + convert.str()))
			break ;
	}
	uid_str = "session_uid=" + convert.str();
	if (uid != _config->cookie_sessions_max)
	{
		(*sessions)[uid_str] = new Cookie(_config);
		(*sessions)[uid_str]->setSessionUID(uid_str);
		//TODO ? see what to do with the header in that case
		(*sessions)[uid_str]->_generation_time = getTime();
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
			// it = (*sessions).erase(it);
			std::map<std::string, Cookie*>::iterator toErase = it++;
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
	if (this->_life_time == NO_TIMEOUT)
		return (false);
	return ((this->getTime() - this->_generation_time) > this->_config->cookie_life_time);
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

std::string			Cookie::genHeader()
{
	std::string			header;
	std::ostringstream	convert;

	header = "Set-Cookie: " + this->_session_uid;
	header += "; Path=" + this->_path;
	if (this->_http_only)
		header += "; HttpOnly";
	if (this->_secure)
		header += "; Secure";
	convert << this->_config->cookie_life_time;
	header += "; Max-Age=" + convert.str();
	return (header);
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
