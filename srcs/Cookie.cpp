/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:09 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/04 10:45:04 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

std::map<std::string, Cookie *>		Cookie::_sessions;
Config								*Cookie::_config;
bool								Cookie::_is_init = false;

Cookie::Cookie(Config *config)
{
	if (!_is_init)
	{
		std::cout << "Initialisation of cookies" << std::endl;
		_config = config;
		_is_init = true;
	}
	else
		std::cout << "Already init !" << std::endl;
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

Cookie::~Cookie()
{
	/*std::map<std::string, Cookie *>::iterator	it;
	for (it = this->_sessions.begin(); it != this->_sessions.end(); ++it)
	{
		if (it->second != NULL)
		{
			delete it->second;
			this->_sessions[it->first] = NULL;
		}
	}*/
}

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
	this->_sessions[uid]->_session_uid = uid;
}

std::string			Cookie::getSessionUID() const
{
	return (this->_session_uid);
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
				field_value = field_split[i].erase(0, pos + 1);
				writeAttribute(field_name, field_value);
			}
		}
	}
	this->_generation_time = this->getTime();
	if (this->_generation_time == -1)
		return (SERV_ERROR);
	return (0);
}

Cookie		*Cookie::getSession(std::map<std::string, std::string> header)
{
	std::string		uid = "";
	Cookie			*found = NULL;

	std::cout << "step 0" << std::endl;
	removeExpired();
	std::cout << "step 1" << std::endl;
	if (header.find("Cookie") != header.end())
	{
		std::cout << "step 2" << std::endl;
		std::vector<std::string>	cookie_in = stringSplit(header["Cookie"], ";");
		if (_sessions.find(cookie_in[0]) != _sessions.end())
		{
			std::cout << "step 3" << std::endl;
			std::string					uid = cookie_in[0];
			std::vector<std::string>	cookie_in = stringSplit(header["Cookie"], "=");
			if (!sessionExists(uid))
			{
				std::cerr << "Invalid session ID, could not retrieve cookies " << sessionExists(uid) << std::endl;
				found = NULL;
			}
			else
			{
				found = _sessions[uid];
				found->updateCookie(header);
				std::cout << "Found cookie session" << std::endl;
			}
		}
	}
	std::cout << "step 5" << std::endl;
	if (found == NULL)
		found = this->createSession(header);
	std::cout << "Found = " << found << std::endl;
	return (found);
}

bool	Cookie::sessionExists(std::string uid)
{
	if (_sessions.find(uid) != _sessions.end())
		return (_sessions[uid] != NULL);
	return (false);
}

Cookie		*Cookie::createSession(std::map<std::string, std::string> header)
{
	int 				uid = 0;
	std::string			uid_str;
	std::ostringstream	convert;

	for (int uid = 0; uid < this->_config->cookie_sessions_max; ++uid)
	{
		convert.str("");
		convert << uid;
		if (!sessionExists("session_uid=" + convert.str()))
			break ;
	}
	uid_str = "session_uid=" + convert.str();
	if (uid != this->_config->cookie_sessions_max)
	{
		_sessions[uid_str] = new Cookie(_config);
		_sessions[uid_str]->setSessionUID(uid_str);
		//_sessions[uid_str]->updateCookie(header);
		(void)header;
		_sessions[uid_str]->_generation_time = this->getTime();
		std::cout << "uid_str = " << uid_str << ", ptr = " << _sessions[uid_str] << std::endl;
		return (_sessions[uid_str]);
	}
	std::cerr << "Cannot create new Cookie, limit reached" << std::endl;
	return (NULL);
}

void		Cookie::removeExpired()
{
	std::map<std::string, Cookie *>::iterator	it;
	for (it = _sessions.begin(); it != _sessions.end(); ++it)
	{
		if (it->second != NULL && this != it->second && it->second->isExpired())
		{
			delete _sessions[it->first];
			_sessions[it->first] = NULL;
			it = _sessions.erase(it);
		}
	}
	return ;
}

int		Cookie::removeSession(std::string uid)
{
	if (sessionExists(uid))
	{
		std::cout << "removing " << uid << " uid = " << _sessions[uid]->getSessionUID() << " ptr = " << _sessions[uid] << std::endl;
		delete _sessions[uid];
		_sessions[uid] = NULL;
		_sessions.erase(uid);
		return (0);
	}
	return (-1);
}

Cookie			*Cookie::getSessionByUID(std::string uid)
{
	if (sessionExists(uid))
		return (_sessions[uid]);
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
	std::cout << "time = " << this->getTime() << ", gen time = " << this->_generation_time << ", cookie lifetime = " << this->_config->cookie_life_time << std::endl;
	return ((this->getTime() - this->_generation_time) > this->_config->cookie_life_time);
}

int		Cookie::getTime() const
{
	time_t		t;
	
	t = time(NULL);
	if (t == -1)
	{
		std::cerr << "Clock error in Cookie. Considering Cookie as expired" << std::endl;
		return (-1);
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
	convert << this->_config->cookie_life_time;
	header += "; Max-Age=" + convert.str();
	return (header);
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
