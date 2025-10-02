/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:30 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/02 21:29:25 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Query.hpp"

Query::Query(Config *config, Client *client) : _config(config), _client(client)
{
	this->_statusCode = OK;
	this->_query = new Request();
}

Query::Query(const Query &other) : _query(other._query), _statusCode(other._statusCode) {}

Query &Query::operator=(const Query &other)
{
	if (this != &other)
	{
		this->_config = other._config;
		this->_client = other._client;
		this->_query = other._query;
		this->_statusCode = other._statusCode;
	}
	return (*this);
}

Query::~Query(void)
{
	if (this->_query)
		delete this->_query;
}

int		Query::queryRespond(void)
{
	this->readRequest();
	if (this->_requestStr.length() == 0)
	{
		std::cerr << "queryRespond: Could not retrieve query" << std::endl;
		return (SERV_ERROR);
	}
	this->_query->parseRequest(this->_requestStr);
	if (this->_query->getError() != NONE)
	{
		//TODO send 404 ou chais pas quoi
		std::cerr << "Bad request 403" << std::endl;
		return (-1);
	}
	if (this->setCookie() == SERV_ERROR)
	{
		std::cerr << "Cookie failed" << std::endl;
	}
	this->setRessource();
	//TODO add some funcs
	return ((this->*_queryExecute[std::min(static_cast<int>(this->_query->getMethod()), (int)ERROR)])());
}

int		Query::setCookie()
{
	//TODO code this function
	if (!Cookie::isInit())
		Cookie::initCookies(this->_config);
	this->_cookie = Cookie::findSession(this->_query->getHeaders());
	if (this->_cookie == NULL)
		this->_cookie = Cookie::createSession(this->_query->getHeaders());
	return (0);
}

int		Query::readRequest(void)
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
		this->_requestStr += std::string(buffer).substr(0, bytes_read);
	}
	return (0);
}

int		Query::queryGet(void)
{
	if (!(this->_ressourceStatus & PERM_ROK))
	{
		this->_statusCode = FORBIDDEN;
		return (SERV_ERROR);
	}
	if ((this->_ressourceStatus & IS_DIR))
	{
		this->_statusCode = NOT_FOUND;
		return (SERV_ERROR);
	}
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
	std::cout << "Query answered with code " << this->_statusCode << std::endl;
	return (0);
}

int		Query::queryPost(void)
{
	return (0);
}

int		Query::queryDelete(void)
{
	return (0);
}

int		Query::queryCGIRun(void)
{
	return (0);
}

int		Query::queryError(void)
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
	
	this->_ressource = this->_config->server_home + this->_ressource;
	if (stat(this->_ressource.c_str(), &file_stat) == -1)
	{
		if (errno == EACCES)
			this->_statusCode = FORBIDDEN;
		else
			this->_statusCode = NOT_FOUND;
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
			std::cerr << "Ressource is a directory " << this->_ressource << std::endl;
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
			this->_statusCode = FORBIDDEN;
			this->_ressourceStatus = PERM_ISSUE;
		}
		else
		{
			this->_statusCode = NOT_FOUND;
			this->_ressourceStatus = RESOURCE_NOT_FOUND;
		}
		std::cerr << "Ressource stat failed: " << strerror(errno) << std::endl;
		std::cerr << "Failed to find " << this->_ressource << std::endl;
		return (SERV_ERROR);
	}
	this->_ressourceStatus = EXISTS;
	if (file_stat.st_mode & S_IRUSR)
		this->_ressourceStatus |= PERM_ROK;
	if (file_stat.st_mode & S_IWUSR)
		this->_ressourceStatus |= PERM_WOK;
	if (file_stat.st_mode & S_IXUSR)
		this->_ressourceStatus |= PERM_XOK;
	if (this->_ressourceStatus <= IS_DIR)
		this->_ressourceStatus |= PERM_ISSUE;
	if (endsWith(this->_ressource, ".py"))
	{
		this->_ressourceStatus |= IS_CGI;
		this->_query->setMethod(CGI_RUN);
		this->_contentType = CGI_PY;
	}
	else if (endsWith(this->_ressource, ".php"))
	{
		this->_ressourceStatus |= IS_CGI;
		this->_query->setMethod(CGI_RUN);
	}
	this->_contentLen = file_stat.st_size;
	return (this->_ressourceStatus);
}

std::string	Query::getRessourceTypeStr(void)
{
	if (this->_contentType == HTML)
		return ("text/html");
	if (this->_contentType == PLAIN)
		return ("text/plain");
	if (this->_contentType == JPEG)
		return ("image/jpeg");
	if (this->_contentType == PNG)
		return ("image/png");
	return ("");
}

std::string	Query::getRessourceTypeExtenssion()
{
	if (this->_contentType == HTML)
		return (".html");
	if (this->_contentType == PLAIN)
		return ("");
	if (this->_contentType == JPEG)
		return (".jpeg");
	if (this->_contentType == PNG)
		return (".png");
	if (this->_contentType == CGI_PY)
		return (".py");
	if (this->_contentType == CGI_PHP)
		return (".php");
	
	return ("");
}

void		Query::setHeader()
{
	this->_header = "HTTP/1.0 "
		+ toString(this->_statusCode)
		+ " " + httpStatusToStr(this->_statusCode) + "\r\n";
	this->_header += "Server: Apache/1.3.29 (Unix)\r\n";
	this->_header += this->_cookie->genHeader() + "\r\n";
	this->_header += "Content-Type: " + this->getRessourceTypeStr() + "\r\n";
	this->_header += "Content-Length: " + toString(this->_contentLen) + "\r\n\r\n";
}

int		Query::sendHeader(void)
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

std::string	Query::httpStatusToStr(HttpStatus code)
{
	switch(code)
	{
		case OK:							return "OK";
		case BAD_REQUEST:					return "Bad Request";
		case FORBIDDEN:						return "Forbidden";
		case NOT_FOUND:						return "Not Found";
		case INTERNAL_SERVER_ERROR:			return "Internal Server Error";
		case NOT_IMPLEMENTED:				return "Not Implemented";
		case HTTP_VERSION_NOT_SUPPORTED:	return "HTTP Version Not Supported";
		default:							return "Unknown";
	}
}

std::string	Query::getDefaultErrorPage(HttpStatus code)
{
	switch(code)
	{
		case BAD_REQUEST:					return ERROR_PAGE_400;
		case FORBIDDEN:						return ERROR_PAGE_403;
		case NOT_FOUND:						return ERROR_PAGE_404;
		case INTERNAL_SERVER_ERROR:			return ERROR_PAGE_500;
		case NOT_IMPLEMENTED:				return ERROR_PAGE_501;
		case HTTP_VERSION_NOT_SUPPORTED:	return ERROR_PAGE_505;
		default:							return "<html><body><h1>Unknown Error</h1></body></html>";
	}
}

const Query::queryMethod	Query::_queryExecute[_methodNum] = {
	&Query::queryGet,
	&Query::queryPost, 
	&Query::queryDelete,
	&Query::queryCGIRun,
	&Query::queryError
};
