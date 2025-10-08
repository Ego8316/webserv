/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:30 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/09 01:08:24 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Query.hpp"

Query::Query(Config *config, Client *client, std::map<std::string, Cookie *> *all_cookies) : _config(config), _client(client), _all_cookies(all_cookies)
{
	this->_err_code = 200;
	this->_query = new Request(this->_all_cookies);
}

Query::Query(const Query &other) :  _query(other._query), _err_code(other._err_code), _config(other._config), _client(other._client) {}

Query &Query::operator=(const Query &other)
{
	if (this != &other)
	{
		this->_config = other._config;
		this->_client = other._client;
		this->_query = other._query;
		this->_err_code = other._err_code;
	}
	return (*this);
}

Query::~Query()
{
	delete this->_query;
}

int		Query::queryRespond()
{
	this->readRequest();
	std::cout << "REQUEST = " << std::endl;
	std::cout << this->_query_str << std::endl;
	if (this->_query_str.length() == 0)
	{
		std::cerr << "queryRespond: Could not retrieve query" << std::endl;
		return (SERV_ERROR);
	}
	this->_query->parseRequest(this->_query_str);
	if (this->_query->getError() != NONE)
	{
		std::cerr << "Bad request 403" << std::endl;
		this->_err_code = 403;
		return (this->queryError());
	}
	this->_query_cookies = this->_query->getQueryCookies();
	if (this->_query_cookies.size() == 0)
		std::cerr << "Cookie failed" << std::endl;
	if (this->setRessource() == SERV_ERROR)
		return (SERV_ERROR);
	this->screenErrors();
	if (this->_err_code != 200)
		return (this->queryError());
	if (this->_ressource_status & IS_DIR)
		return (this->queryListDir());
	//TODO add some funcs ?
	return ((this->*_queryExecute[std::min(static_cast<int>(this->_query->getMethod()), (int)ERROR)])());
}

void	Query::screenErrors()
{
	if (this->_err_code != 200)
		return ;
	//TODO
	return ;
}

int		Query::readRequest()
{
	char	buffer[BUFFER_SIZE];
	int		bytes_read;
	
	bytes_read = this->_config->buffer_size;
	while (bytes_read == this->_config->buffer_size)
	{
		bytes_read = this->_client->socketRead(buffer, bytes_read);
		if (bytes_read == SERV_ERROR)
		{
			std::cerr << "Query could not retrive request" << std::endl;
			return (SERV_ERROR);
		}
		this->_query_str += std::string(buffer).substr(0, bytes_read);
	}
	return (0);
}

int		Query::queryGet()
{
	if (!(this->_ressource_status & PERM_ROK))
	{
		this->_err_code = 403;
		return (SERV_ERROR);
	}
	if ((this->_ressource_status & IS_DIR))
		return (SERV_ERROR);
	this->setHeader();
	if (access(this->_ressource.c_str(), R_OK) == 0)
	{
		if (this->sendHeader() == SERV_ERROR)
		{
			std::cerr << "Failed to send header" << std::endl;
			return (SERV_ERROR);
		}
		this->streamFile(this->_ressource);
	}
	else
		std::cerr << "Could not access ressource : >" << this->_ressource << "<" << std::endl;
	std::cout << "Query answered with code " << this->_err_code << std::endl;
	return (0);
}

int		Query::queryPost()
{
	return (0);
}

int		Query::queryDelete()
{
	return (0);
}

int		Query::queryCGIRun()
{
	return (0);
}

int		Query::queryError()
{
	return (0);
}

int		Query::queryListDir()
{
	std::string		ret_body;
	DIR				*dir;
	struct dirent	*dir_ent;
	
	dir = opendir(this->_ressource.c_str());
	if (dir == NULL)
	{
		std::cerr << "Cannot open directory\n" << std::endl;
		return (SERV_ERROR);
	}
	ret_body = LISTDIR_HEADER;
	dir_ent = readdir(dir);
	while (dir_ent != NULL)
	{
		ret_body += LISTDIR_PREFIX + std::string(dir_ent->d_name) + LISTDIR_SUFFIX;
		dir_ent = readdir(dir);
	}
	ret_body += LISTDIR_ENDING;
	closedir(dir);
	this->_content_len = ret_body.length();
	this->setHeader();
	this->sendHeader();
	this->_client->socketWrite(ret_body.c_str(), ret_body.length());
	return (0);
}

int		Query::setRessource()
{
	this->_ressource = this->_query->getRequestTarget();
	if (this->findRessource() == SERV_ERROR)
	{
		std::cerr << "Ressource could not be found" << std::endl;
		return (SERV_ERROR);
	}
	if (this->_err_code != 200)
		return (SERV_ERROR);
	if (this->setRessourceStatus() == SERV_ERROR)
	{
		std::cerr << "Ressource status error " << std::endl;
		return (SERV_ERROR);
	}
	return (0);
}

int		Query::findRessource()
{
	struct stat file_stat;
	struct stat dir_stat;
	
	this->_ressource = this->_config->server_home + this->_ressource;
	if (stat(this->_ressource.c_str(), &file_stat) == -1)
	{
		if (errno == EACCES)
			this->_err_code = 403;
		else
			this->_err_code = 404;
		std::cerr << "Ressource stat failed: " << strerror(errno) << std::endl;
		std::cerr << "Failed to find " << this->_ressource << std::endl;
		return (SERV_ERROR);
	}
	else
	{
		if (S_ISDIR(file_stat.st_mode))
		{
			if (stat((this->_ressource + this->_config->default_page).c_str(), &dir_stat) == 0)
			{
				this->_ressource += this->_config->default_page;
				return (0);
			}
			else if (this->_config->enable_listdir == false)
			{
				std::cerr << "Ressource is a directory " << this->_ressource << std::endl;
				return (SERV_ERROR);
			}
		}
	}
	return (0);
}

int		Query::setRessourceStatus()
{
	struct stat file_stat;

	if (stat(this->_ressource.c_str(), &file_stat) == SERV_ERROR)
	{
		if (errno == EACCES)
		{
			this->_err_code = 403;
			this->_ressource_status = PERM_ISSUE;
		}
		else
		{
			this->_err_code = 404;
			this->_ressource_status = NOT_FOUND;
		}
		std::cerr << "Ressource stat failed: " << strerror(errno) << std::endl;
		std::cerr << "Failed to find " << this->_ressource << std::endl;
		return (SERV_ERROR);
	}
	this->_ressource_status = EXISTS;
	if (S_ISDIR(file_stat.st_mode))
		this->_ressource_status |= IS_DIR;
	if (file_stat.st_mode & S_IRUSR)
		this->_ressource_status |= PERM_ROK;
	if (file_stat.st_mode & S_IWUSR)
		this->_ressource_status |= PERM_WOK;
	if (file_stat.st_mode & S_IXUSR)
		this->_ressource_status |= PERM_XOK;
	if (this->_ressource_status <= IS_DIR)
		this->_ressource_status |= PERM_ISSUE;
	if (endsWith(this->_ressource, ".py"))
	{
		this->_ressource_status |= IS_CGI;
		this->_query->setMethod(CGI_RUN);
		this->_content_type = ACCEPT_CGI_PY;
	}
	else if (endsWith(this->_ressource, ".php"))
	{
		this->_ressource_status |= IS_CGI;
		this->_query->setMethod(CGI_RUN);
		this->_content_type = ACCEPT_CGI_PHP;
	}
	else if (endsWith(this->_ressource, ".html"))
		this->_content_type = ACCEPT_HTML;
	else if (endsWith(this->_ressource, ".jpeg"))
		this->_content_type = ACCEPT_JPEG;
	else if (endsWith(this->_ressource, ".png"))
		this->_content_type = ACCEPT_PNG;
	else if (endsWith(this->_ressource, ".txt") || this->_ressource.find(".") == std::string::npos)
		this->_content_type = ACCEPT_PLAIN;
	this->_content_len = file_stat.st_size;
	//TODO check Accept ici
	return (0);
}

std::string	Query::getRessourceTypeStr()
{
	if (this->_content_type == ACCEPT_HTML)
		return ("text/html");
	if (this->_content_type == ACCEPT_PLAIN)
		return ("text/plain");
	if (this->_content_type == ACCEPT_JPEG)
		return ("image/jpeg");
	if (this->_content_type == ACCEPT_PNG)
		return ("image/png");
	return ("");
}

std::string	Query::getRessourceTypeExtenssion()
{
	if (this->_content_type == ACCEPT_HTML)
		return (".html");
	if (this->_content_type == ACCEPT_PLAIN)
		return ("");
	if (this->_content_type == ACCEPT_JPEG)
		return (".jpeg");
	if (this->_content_type == ACCEPT_PNG)
		return (".png");
	if (this->_content_type == ACCEPT_CGI_PY)
		return (".py");
	if (this->_content_type == ACCEPT_CGI_PHP)
		return (".php");
	
	return ("");
}

void		Query::setHeader()
{
	this->_header = "HTTP/1.0 " + std::to_string(this->_err_code) + " OK\r\n";
	this->_header += "Server: Apache/1.3.29 (Unix)\r\n";
	for (unsigned int i = 0; i < this->_query_cookies.size(); ++i)
		this->_header += this->_query_cookies[i]->genHeader() + "\r\n";
	this->_header += "Content-Type: " + this->getRessourceTypeStr() + "\r\n";
	this->_header += "Content-Length: " + std::to_string(this->_content_len) + "\r\n";
	this->_header += "\r\n";
}

int		Query::sendHeader()
{
	if (this->_client->socketWrite(this->_header.c_str(), this->_header.length()) == SERV_ERROR)
	{
		//set err
		return (SERV_ERROR);
	}
	return (0);
}

int		Query::streamFile(std::string file)
{
	int		fd;
	char	buffer[BUFFER_SIZE];
	ssize_t	bytes_read;

	fd = open(file.c_str(), O_RDONLY | O_NONBLOCK);
	bytes_read = read(fd, buffer, sizeof(buffer));
	if (bytes_read <= 0)
	{
		//set err
		return (SERV_ERROR);
	}
	while (bytes_read > 0)
	{
		if (send(this->_client->getFd(), buffer, bytes_read, 0) == -1)
		{
			//set err
			return (SERV_ERROR);
		}
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == -1)
		{
			//set err
			return (SERV_ERROR);
		}
	}
	close(fd);
	return (0);
}

const Query::queryMethod	Query::_queryExecute[_method_num] = {
	&Query::queryGet,
	&Query::queryPost, 
	&Query::queryDelete,
	&Query::queryCGIRun,
	&Query::queryError
};