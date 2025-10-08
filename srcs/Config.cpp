/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/08 14:03:59 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(std::string config)
{
	std::istringstream	conf_stream(config);
	std::string			newline;
	std::string			field, equal, value;

	this->parse_error = NONE;
	while (std::getline(conf_stream, newline))
	{
		std::istringstream 			line(newline);
	
		if (!(line >> field >> equal >> value) || (equal != "=" && equal != ":"))
		{
			std::cerr << "Could not parse config line " << newline << std::endl;
			continue;
		}
		if (field == "IP")
			this->setIP(value);
		if (field == "PORT")
			this->port_number = atoi(value.c_str());
		else if (field == "DOMAIN")
		{
			if (value == "AF_INET")
				this->domain = AF_INET;
		}
		else if (field == "TYPE")
		{
			if (value == "SOCK_STREAM")
				this->type = SOCK_STREAM;
		}
		else if (field == "PROTOCOL")
			this->protocol = atoi(value.c_str());
		else if (field == "CLIENT_LIMIT")
			this->client_limit = atoi(value.c_str());
		else if (field == "COOKIE_LIFE_TIME")
			this->cookie_life_time = atoi(value.c_str());
		else if (field == "COOKIE_SESSIONS_MAX")
			this->cookie_sessions_max = atoi(value.c_str());
		else if (field == "BUFFER_SIZE")
			this->buffer_size = atoi(value.c_str());
		else if (field == "SERVER_HOME")
			this->server_home = value;
		else if (field == "DEFAULT_PAGE")
			this->default_page = value;
		else if (field == "INCOMMING_QUEUE_BACKLOG")
			this->incomming_queue_backlog = atoi(value.c_str());
		else if (field == "DEFAULT_ERROR_PAGES" && value == "list")
			this->parseDefaultErrorPages(conf_stream);
	}
}

Config::Config(const Config &other)
{
	this->port_number = other.port_number;
	this->domain = other.domain;
	this->type = other.type;
	this->protocol = other.protocol;
	this->client_limit = other.client_limit;
	this->buffer_size = other.buffer_size;
	this->server_home = other.server_home;
}

Config &Config::operator=(const Config &other)
{
	if (this != &other)
	{
		this->port_number = other.port_number;
		this->domain = other.domain;
		this->type = other.type;
		this->protocol = other.protocol;
		this->client_limit = other.client_limit;
		this->buffer_size = other.buffer_size;
		this->server_home = other.server_home;
	}
	return (*this);
}

Config::~Config() {}


ParseError		Config::getParseError()
{
	return (this->parse_error);
}

void			Config::setIP(std::string ip_str)
{
	int octets[4];
    char dot;
    std::istringstream iss(ip_str);
	unsigned int	ip_uint = 0;
	
	if (ip_str == "INADDR_ANY")
	{
		this->ip = INADDR_ANY;
		return ;
	}
	else if (ip_str == "INADDR_NONE")
	{
		this->ip = INADDR_NONE;
		return ;
	}
    if ((iss >> octets[0] >> dot >> octets[1] >> dot >> octets[2] >> dot >> octets[3])
		&& dot == '.' && iss.eof())
	{
        for (int i = 0; i < 4; ++i)
		{
            if (octets[i] < 0 || octets[i] > 255)
			{
				std::cerr << "Could not parse IP address: " << ip_str << " defaulting to INADDR_ANY" << std::endl;
				this->ip = INADDR_ANY;
				return ;
			}
			ip_uint += octets[i] * pow(2, (3 - i) * 8);
        }
	}
	this->ip = htonl(ip_uint);
}

void		Config::parseDefaultErrorPages(std::istringstream &conf_stream)
{
	std::string			newline;
	int					field;
	std::string			equal, value;

	while (std::getline(conf_stream, newline))
	{
		if (newline.find("end") != std::string::npos)
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> field >> equal >> value) || (equal != "=" && equal != ":"))
		{
			std::cerr << "Could not parse config line " << newline << std::endl;
			continue;
		}
		if (field < 200 || field > 600)
		{
			std::cerr << "Unknown error number " << field << " in Default Error Page définitions" << std::endl;
			continue;
		}
		this->default_error_pages[field] = value;
	}
}

std::ostream	&operator<<(std::ostream &os, const Config &item)
{
	os << "IP :" << item.ip << std::endl;
	os << "port_number :" << item.port_number << std::endl;
	os << "incomming_queue_backlog :" << item.incomming_queue_backlog << std::endl;
	os << "client_limit :" << item.client_limit << std::endl;
	os << "domain :" << item.domain << std::endl;
	os << "type :" << item.type << std::endl;
	os << "protocol :" << item.protocol << std::endl;
	os << "buffer_size :" << item.buffer_size << std::endl;
	os << "ServHome :" << item.server_home << std::endl;
	os << "default_page :" << item.default_page << std::endl;
	os << "parse_error :" << item.parse_error << std::endl;
	os << "cookie_life_time :" << item.cookie_life_time << std::endl;
	os << "cookie_sessions_max :" << item.cookie_sessions_max << std::endl;
	return (os);
}

std::vector<Config *> Config::parseMultipleConfigs(std::string filename)
{
	std::ifstream 			conf_file;
	std::string				newline;
	std::vector<Config *>	configs;
	std::string				config_section;
	
	conf_file.open(filename.c_str(), O_RDONLY);
	
	configs.resize(0);
	if (!conf_file.is_open())
	{
		std::cerr << "Could not open config file" << std::endl;
		return (configs);
	}
	while (std::getline(conf_file, newline))
	{
		if (newline.find("{") != std::string::npos)
		{
			if (config_section.length() != 0)
			{
				std::cerr << "Nested configs not supported" << std::endl;
				deleteAllConfigs(configs);
				return (configs);
			}
			else if (newline.find("}") != std::string::npos)
			{
				std::cerr << "Format not recognized" << std::endl;
				deleteAllConfigs(configs);
				return (configs);
			}
			else
				config_section = newline.erase(0, newline.find("{") + 1) + "\n";
		}
		else if (newline.find("}") != std::string::npos)
		{
			config_section += newline.substr(0, newline.find("}")) + "\n";
			configs.push_back(new Config(config_section));
			std::cout << *configs[0] << std::endl;
			config_section = "";
		}
		else if (newline.length())
			config_section += newline + "\n";
	}
	conf_file.close();
	return (configs);
}

void	Config::deleteAllConfigs(std::vector<Config *> &configs)
{
	for (unsigned int i = 0; i < configs.size(); ++i)
	{
		if (configs[i] != NULL)
			delete configs[i];
	}
	configs.resize(0);
}