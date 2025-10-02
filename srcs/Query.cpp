/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:30 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/02 15:32:04 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Query.hpp"

Query::Query(Config *config, Client *client) : _config(config), _client(client)
{
	this->_err_code = 200;
	this->_query = new Request();
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
	if (this->_query_str.length() == 0)
	{
		std::cerr << "queryRespond: Could not retrieve query" << std::endl;
		return (SERV_ERROR);
	}
	this->_query->parseRequest(this->_query_str);
	if (this->_query->getError() != NONE)
	{
		//TODO send 404 ou chais pas quoi
		std::cerr << "Bad request 403" << std::endl;
		return (-1);
	}
	std::cout << "version is now " << this->_query->getVersion() << std::endl;
	std::cout << "We good ?" << std::endl;
	if (this->setCookie() == SERV_ERROR)
	{
		std::cerr << "Cookie failed" << std::endl;
	}
	std::cout << "*clocks gun* I said we good ?" << std::endl;
	if (this->_cookie == NULL)
	{
		std::cerr << "Could not initiate Cookies" << std::endl;
		this->_cookie = Cookie(this->_config).createSession();
		if (this->_cookie == NULL)
		{
			std::cerr << "Fatal: cannot initiate Cookies" << std::endl;
			return (SERV_ERROR);
		}
	}
	std::cout << "How about now ?" << std::endl;
	this->setRessource();
	//TODO add some funcs
	std::cout << "And here ?" << std::endl;
	return ((this->*_queryExecute[std::min(static_cast<int>(this->_query->getMethod()), (int)ERROR)])());
}

int		Query::setCookie()
{
	//TODO code this function
	std::cout << *this->_query << std::endl;
	Cookie(this->_config); //TODO : trouver une meilleur manière de faire ca...
	this->_cookie = Cookie::findSession(this->_query->getHeaders());
	if (this->_cookie == NULL)
	{
		std::cout << "creating new Cookie" << std::endl;
		this->_cookie = new Cookie();
	}
	std::cout << "hello ?" << std::endl;
	std::cout << "Printing cookies !!!" << std::endl;
	std::cout << *(this->_cookie) << std::endl;
	std::cout << "ok so we are ok" << std::endl;
	return (0);
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
	std::cout << "step 1" << std::endl;
	if (!(this->_ressource_status & PERM_ROK))
	{
		this->_err_code = 403;
		return (SERV_ERROR);
	}
	std::cout << "step 2" << std::endl;
	if ((this->_ressource_status & IS_DIR))
	{
		this->_err_code = 404;
		return (SERV_ERROR);
	}
	std::cout << "step 3" << std::endl;
	this->setHeader();
	std::cout << "step 4" << std::endl;
	if (access(this->_ressource.c_str(), R_OK) == 0)
	{
		std::cout << "step 5" << std::endl;
		if (this->sendHeader() == SERV_ERROR)
		{
			std::cerr << "Failed to send header" << std::endl;
			return (SERV_ERROR);
		}
		this->streamFile(this->_ressource);
	}
	else
		std::cerr << "Could not access ressource : >" << this->_ressource << "<" << std::endl;
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

int		Query::setRessource()
{
	this->_ressource = this->_query->getRequestTarget();
	if (this->findRessource() == -1)
	{
		std::cerr << "Ressource could not be found" << std::endl;
		return (SERV_ERROR);
	}
	if (this->setRessourceStatus() == -1)
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
	
	if (stat(this->_ressource.c_str(), &file_stat) == -1)
	{
		if (errno == EACCES)
			this->_err_code = 403;
		else
			this->_err_code = 404;
		std::cerr << "Ressource stat failed: " << strerror(errno) << std::endl;
		std::cerr << "Failed to find " << this->_ressource << std::endl;
		std::cerr << "Failed to find " << this->_query->getRequestTarget() << std::endl;
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
			std::cerr << "Ressource is a directory " << this->_query->getRequestTarget() << std::endl;
			return (SERV_ERROR);
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
		std::cerr << "Failed to find " << this->_query->getRequestTarget() << std::endl;
		std::cerr << "Failed to find " << this->_ressource << std::endl;
		return (SERV_ERROR);
	}
	this->_ressource_status = EXISTS;
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
		this->_content_type = CGI_PY;
	}
	else if (endsWith(this->_ressource, ".php"))
	{
		this->_ressource_status |= IS_CGI;
		this->_query->setMethod(CGI_RUN);
		this->_content_type = CGI_PHP;
	}
	this->_content_len = file_stat.st_size;
	return (0);
}

std::string	Query::getRessourceTypeStr()
{
	if (this->_content_type == HTML)
		return ("text/html");
	if (this->_content_type == PLAIN)
		return ("text/plain");
	if (this->_content_type == JPEG)
		return ("image/jpeg");
	if (this->_content_type == PNG)
		return ("image/png");
	return ("");
}

std::string	Query::getRessourceTypeExtenssion()
{
	if (this->_content_type == HTML)
		return (".html");
	if (this->_content_type == PLAIN)
		return ("");
	if (this->_content_type == JPEG)
		return (".jpeg");
	if (this->_content_type == PNG)
		return (".png");
	if (this->_content_type == CGI_PY)
		return (".py");
	if (this->_content_type == CGI_PHP)
		return (".php");
	
	return ("");
}

void		Query::setHeader()
{
	std::cout << "step 3.1" << std::endl;
	this->_header = "HTTP/1.0 " + std::to_string(this->_err_code) + " OK\r\n";
	std::cout << "step 3.2" << std::endl;
	this->_header += "Server: Apache/1.3.29 (Unix)\r\n";
	std::cout << "step 3.3" << std::endl;
	this->_header += this->_cookie->genHeader() + "\r\n";
	std::cout << "step 3.4" << std::endl;
	this->_header += "Content-Type: " + this->getRessourceTypeStr() + "\r\n";
	std::cout << "step 3.5" << std::endl;
	this->_header += "Content-Length: " + std::to_string(this->_content_len) + "\r\n";
	std::cout << "step 3.6" << std::endl;
	this->_header += "\r\n";
}

int		Query::sendHeader()
{
	if (this->_client->socketWrite(this->_header.c_str(), this->_header.length()) == -1)
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