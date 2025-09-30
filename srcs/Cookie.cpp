/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:09 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 14:42:03 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

std::vector<Cookie*> Cookie::_sessions;

Cookie::Cookie()
{
	this->initCookies();
}

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

Cookie::~Cookie() {}

int		Cookie::initCookies()
{
	if (Cookie::_sessions.size() != MAX_COOKIE_SESSIONS)
	{
		Cookie::_sessions.resize(MAX_COOKIE_SESSIONS);
		return (1);
	}
	return (0);
}

bool			Cookie::hasField(std::string key)
{
	if (this->_cookies.find(key) == this->_cookies.end())
		return (false);
	return (true);
}

std::string		Cookie::getField(std::string key)
{
	if (this->hasField(key))
		return (this->_cookies[key]);
	return ("");
}

void		Cookie::writeField(std::string key, std::string newvalue)
{
	if (this->getField(key).compare(newvalue) == 0)
		return ;
	this->_cookies[key] = newvalue;
}

void		Cookie::appendField(std::string key, std::string newvalue)
{
	if (this->getField(key).find(newvalue) != this->getField(key).end())
		return ;
	this->_cookies[key] = this->_cookies[key] + "; " + newvalue;
}

void		Cookie::setSessionID(unsigned int id)
{
	this->_session_id = id;
}

Cookie			*Cookie::getCookie(std::map<std::string, std::string> header)
{
	//TODO check if user exist and return the appropriate cookike;
	int		uid;
	
	uid = findSession(header);
	if (uid == -1)
	{
		uid = createSession();
		if (uid == -1)
			return (NULL);
	}
	Cookie::_sessions[uid]->updateCookie(header);
	return (Cookie::_sessions[uid]);
}

int			Cookie::updateCookie(std::map<std::string, std::string> header)
{
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end() ; ++it)
	{
		if (it->first.compare("Set-Cookie") == 0)
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
				this->writeField(field_name, field_value);
			}
		}
	}
	//TODO set _life_time pour le cookie
	return (0);
}

int			Cookie::findSession(std::map<std::string, std::string> header)
{
	int		uid = -1;
	
	if (header.find("Set-Cookie") != header.end())
	{
		std::vector<std::string>	cookie_in = stringSplit(header["Set-Cookie"], ";");
		if (cookie_in[0].compare(0, 11, "session_id") == 0)
		{
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
	if (id >= 0 && id < MAX_COOKIE_SESSIONS)
		return (_sessions[id] != NULL);
	return (false);
}

int		Cookie::createSession()
{
	for (int i = 0; i < MAX_COOKIE_SESSIONS; ++i)
	{
		if (!sessionExists(i))
		{
			_sessions[i] = new Cookie();
			_sessions[i]->setSessionID(i);
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

int		Cookie::isExpired()
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
	return (difftime(t, this->_life_time) > 0);
}
