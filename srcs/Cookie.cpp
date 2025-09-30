/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:09 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 23:09:46 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

Cookie	*Cookie::_sessions[MAX_COOKIE_SESSIONS];

Cookie::Cookie() {}

Cookie::Cookie(const Cookie &other) : _session_id(other._session_id), _life_time(other._life_time) {}

Cookie &Cookie::operator=(const Cookie &other)
{
	if (this != &other)
	{
		this->_session_id = other._session_id;
		this->_life_time = other._life_time;
	}
	return (*this);
}

Cookie::~Cookie()
{
	for (unsigned int i = 0; i < MAX_COOKIE_SESSIONS; ++i)
	{
		if (this->_sessions[i] != NULL)
		{
			delete this->_sessions[i];
		}
	}
}

bool		Cookie::hasAttribute(std::string key) const
{
	if (this->_attributes.find(key) == this->_attributes.end())
		return (false);
	return (true);
}

void		Cookie::writeAttribute(std::string key, std::string newvalue)
{
	if (this->getAttribute(key).compare(newvalue) == 0)
		return ;
	this->_attributes[key] = newvalue;
}

void		Cookie::appendAttribute(std::string key, std::string newvalue)
{
	if (this->hasAttribute(key))
		this->_attributes[key] = this->_attributes[key] + "; " + newvalue;
	else
		this->_attributes[key] = newvalue;
}

void		Cookie::setSessionId(unsigned int id)
{
	this->_session_id = id;
}

int			Cookie::getSessionId() const
{
	return (this->_session_id);
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

Cookie			*Cookie::getCookie(std::map<std::string, std::string> header)
{
	int		uid;

	//initCookies();
	uid = findSession(header);
	if (uid == -1)
	{
		std::cerr << "Could not find Cookie session" << std::endl;
		uid = createSession();
		if (uid == -1)
		{
			std::cerr << "Could not initiate Cookie session" << std::endl;
			return (NULL);
		}
	}
	Cookie::_sessions[uid]->updateCookie(header);
	return (Cookie::_sessions[uid]);
}


int			Cookie::updateCookie(std::map<std::string, std::string> header)
{
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end() ; ++it)
	{
		if (it->first.compare("Cookie") == 0)
		{
			std::vector<std::string>	field_split = stringSplit(it->second, "; ");
			std::string					field_name;
			std::string					field_value;
			size_t						pos;
			
			for (unsigned int i = 0; i < field_split.size(); ++i)
			{
				pos = field_split[i].find("=");
				field_name = field_split[i].substr(0, pos);
				field_value = field_split[i].erase(0, pos);
				this->writeAttribute(field_name, field_value);
			}
		}
	}
	this->_generation_time = this->getTime();
	if (this->_generation_time == -1)
		return (SERV_ERROR);
	return (0);
}

int			Cookie::findSession(std::map<std::string, std::string> header)
{
	int		uid = -1;
	
	if (header.find("Cookie") != header.end())
	{
		std::vector<std::string>	cookie_in = stringSplit(header["Cookie"], ";");
		if (cookie_in[0].compare(0, 11, "session_id") == 0)
		{
			std::cout << "Is this your session id ? >" + cookie_in[0].substr(11, cookie_in[0].length()) + "<";
			uid = std::atoi(cookie_in[0].substr(11, cookie_in[0].length()).c_str());
			if (uid < 0 || uid >= MAX_COOKIE_SESSIONS || !sessionExists(uid))
			{
				std::cerr << "Invalid session ID, could not retrieve cookies";
				uid = -1;
			}
			else if (sessionExists(uid) && _sessions[uid]->isExpired())
			{
				removeSession(uid);
				uid = -1;
			}
		}
	}
	return (uid);
}

bool	Cookie::sessionExists(int id)
{
	//initCookies();
	if (id >= 0 && id < MAX_COOKIE_SESSIONS)
		return (_sessions[id] != NULL);
	return (false);
}

int		Cookie::createSession()
{
	//initCookies();
	for (int i = 0; i < MAX_COOKIE_SESSIONS; ++i)
	{
		if (_sessions[i] == NULL)
		{
			_sessions[i] = new Cookie();
			_sessions[i]->setSessionId(0);
			return (i);
		}
	}
	return (0);
}

int		Cookie::removeSession(int id)
{
	if (sessionExists(id))
	{
		delete _sessions[id];
		_sessions[id] = NULL;
		return (0);
	}
	return (-1);
}

Cookie			*Cookie::getSessionById(unsigned int idx)
{
	if (sessionExists(idx))
		return (_sessions[idx]);
	return (NULL);
}

bool		Cookie::isExpired() const
{
	time_t	t;

	if (this->_life_time == NO_TIMEOUT)
		return (false);
	t = time(NULL);
	if (t == -1)
	{
		std::cerr << "Clock error in Cookie. Considering Cookie as expired" << std::endl;
		return (true);
	}
	return ((this->_generation_time + COOKIE_LIFE_TIME - this->getTime()) > 0);
}

int		Cookie::getTime() const
{
	time_t	t;
	
	t = time(NULL);
	if (t == -1)
	{
		std::cerr << "Clock error in Cookie. Considering Cookie as expired" << std::endl;
		return (-1);
	}
	return (t);
}

std::string			Cookie::genHeader()
{
	std::string	header;

	header = "Set-Cookie: session_id=" + std::to_string(this->_session_id);
	header += "; Path=" + this->getAttribute("Path");
	if (this->_http_only)
		header += "; HttpOnly";
	header += "; Max-Age=" + std::to_string(this->_generation_time + COOKIE_LIFE_TIME - this->getTime());
	return (header);
}


std::ostream	&operator<<(std::ostream &os, const Cookie &item)
{
	std::map<std::string, std::string>				attr = item.getAllAttributes();
	std::map<std::string, std::string>::iterator	it;
	os << "Cookie :" << std::endl;
	os << "session id = :\t\t" << item.getSessionId() << std::endl;
	os << "attributes:" << std::endl;
	for (it = attr.begin(); it != attr.end(); ++it)
		os << it->first << " : " << it->second;
	os << "End attributes" << std::endl;
	return (os);
}
