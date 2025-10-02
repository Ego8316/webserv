/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:21:09 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/02 16:23:46 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

std::vector<Cookie *>	Cookie::_sessions;
bool					Cookie::_is_init = false;
Config					*Cookie::_config;

Cookie::Cookie() {}

Cookie::Cookie(Config *config)
{
	if (!_is_init)
	{
		std::cout << "Initialisation of cookies" << std::endl;
		_config = config;
		_sessions.resize(this->_config->cookie_sessions_max);
		_is_init = true;
	}
	else
		std::cout << "Already init !" << std::endl;
	return ;
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
	if (this->getAttribute(key) == newvalue)
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

bool		Cookie::isInit()
{
	return (_is_init);
}

void		Cookie::initCookies(Config *config)
{
	if (!isInit())
	{
		std::cout << "Initialisation of cookies" << std::endl;
		_config = config;
		_sessions.resize(_config->cookie_sessions_max);
		_is_init = true;
	}
	else
		std::cout << "Already init !" << std::endl;
}

std::map<std::string, std::string> const	&Cookie::getAllAttributes() const
{
	return (this->_attributes);
}

/*Cookie			*Cookie::getCookie(std::map<std::string, std::string> header)
{
	Cookie	*session_cookie;

	removeExpired();
	session_cookie = findSession(header);
	if (session_cookie == NULL)
	{
		std::cerr << "Could not find Cookie session" << std::endl;
		session_cookie = createSession();
		if (session_cookie == NULL)
		{
			std::cerr << "Could not initiate Cookie session" << std::endl;
			return (NULL);
		}
		else
			std::cout << "creation of session_cookie successful " << session_cookie << " " << session_cookie->getSessionId() << std::endl;
	}
	else
		std::cout << "Found User cookies !" << std::endl;
	if (session_cookie->updateCookie(header) == -1)
	{
		std::cerr << "Updating of cookies failed" << std::endl;
		removeSession(session_cookie->getSessionId());
		return (NULL);
	}
	std::cout << "coucou ! " << session_cookie << " " << session_cookie->getSessionId() << std::endl;
	return (session_cookie);
}*/


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
				this->writeAttribute(field_name, field_value);
			}
		}
	}
	this->_generation_time = this->getTime();
	std::cout << "generation time is " << this->_generation_time << std::endl;
	if (this->_generation_time == -1)
		return (SERV_ERROR);
	return (0);
}

Cookie		*Cookie::findSession(std::map<std::string, std::string> header)
{
	int		uid = -1;
	Cookie	*found = NULL;
	
	std::cout << "enter findSession" << std::endl;
	if (header.find("Cookie") != header.end())
	{
		std::vector<std::string>	cookie_in = stringSplit(header["Cookie"], ";");
		if (cookie_in[0].find("session_id", 0) != std::string::npos)
		{
			//uid = std::atoi(cookie_in[0].substr(11, cookie_in[0].length()).c_str());//cookie_in[0].substr(cookie_in[0].find("session_id", 0) + 11, cookie_in[0].length())
			uid = std::atoi(cookie_in[0].substr(cookie_in[0].find("=", 0) + 1, cookie_in[0].length()).c_str());
			if (uid < 0 || uid >= MAX_COOKIE_SESSIONS || !sessionExists(uid))
			{
				std::cerr << "Invalid session ID, could not retrieve cookies " << sessionExists(uid) << std::endl;
				found = NULL;
			}
			else if (sessionExists(uid) && _sessions[uid]->isExpired())
			{
				removeSession(uid);
				found = NULL;
			}
			else
			{
				found = getSessionById(uid);
				found->updateCookie(header);
			}
		}
	}
	std::cout << "exit findSession" << std::endl;
	return (found);
}

bool	Cookie::sessionExists(int id)
{
	removeExpired();
	if (id >= 0 && id < _config->cookie_sessions_max)
		return (_sessions[id] != NULL);
	return (false);
}

Cookie		*Cookie::createSession(std::map<std::string, std::string> header)
{
	removeExpired();
	std::cout << _is_init << std::endl;
	for (int i = 0; i < _config->cookie_sessions_max; ++i)
	{
		if (_sessions[i] == NULL)
		{
			_sessions[i] = new Cookie(_config);
			_sessions[i]->setSessionId(i);
			_sessions[i]->updateCookie(header);
			return (_sessions[i]);
		}
	}
	std::cerr << "Cannot create new Cookie, limit reached" << std::endl;
	return (NULL);
}

void		Cookie::removeExpired()
{
	for (int i = 0; i < _config->cookie_sessions_max; ++i)
	{
		if (_sessions[i] != NULL && _sessions[i]->isExpired())
			removeSession(i);
	}
	return ;
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

	std::cout << "step 3.3.1" << std::endl;
	header = "Set-Cookie: session_id=" + std::to_string(this->_session_id);
	std::cout << "step 3.3.2" << std::endl;
	header += "; Path=" + this->getAttribute("Path");
	std::cout << "step 3.3.3" << std::endl;
	if (this->_http_only)
		header += "; HttpOnly";
	std::cout << "step 3.3.4" << std::endl;
	std::cout << *this->_config << std::endl;
	std::cout << "step 3.3.4.2" << std::endl;
	convert << this->_generation_time + this->_config->cookie_life_time - this->getTime();
	std::cout << "time left = " << this->_generation_time << "+" << this->_config->cookie_life_time << "-" << this->getTime() << " = " << this->_generation_time + this->_config->cookie_life_time - this->getTime() << std::endl;
	header += "; Max-Age=" + convert.str();
	std::cout << "step 3.3.5" << std::endl;
	return (header);
}


std::ostream	&operator<<(std::ostream &os, const Cookie &item)
{
	std::map<std::string, std::string>				attr = item.getAllAttributes();
	std::map<std::string, std::string>::iterator	it;
	os << "Cookie :" << std::endl;
	os << "session id: " << item.getSessionId() << std::endl;
	os << "attributes: " << std::endl;
	for (it = attr.begin(); it != attr.end(); ++it)
		os << it->first << ": " << it->second << std::endl;
	os << "End attributes" << std::endl;
	return (os);
}
