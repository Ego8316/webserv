/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:30 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/29 14:24:50 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Query.hpp"

Query::Query()
{
	this->_err_code = 200;
}

Query::Query(const Query &other) : _query(other._query), _err_code(other._err_code) {}

Query &Query::operator=(const Query &other)
{
	if (this != &other)
	{
		this->_query = other._query;
		this->_err_code = other._err_code;
	}
	return (*this);
}

Query::~Query() {}

int		Query::queryRespond(Client *client, Config *config)
{
	this->_client = client;
	this->_config = config;
	this->readRequest();
	this->_query = Request(this->_query_str);
	if (this->_query.getError() != NONE)
	{
		//TODO send 404 ou chais pas quoi
		std::cerr << "Bad request 403" << std::endl;
		return (-1);
	}
	this->setRessourceStatus();
	if (this->_query_str.length() == 0)
	{
		std::cerr << "queryRespond: Could not retrieve query" << std::endl;
		return (SERV_ERROR);
	}
	this->_query = Request(this->_query_str);
	(this->*_queryExecute[std::min(static_cast<int>(this->_query.getMethod()), (int)ERROR)])();
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
	if (!(this->_ressource_status & PERM_ROK))
	{
		this->_err_code = 403;
		return (SERV_ERROR);
	}
	if ((this->_ressource_status & IS_DIR))
	{
		this->_err_code = 404;
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

int		Query::setRessourceStatus()
{
	struct stat file_stat;
	struct stat dir_stat;

	this->_ressource = this->_query.getRequestTarget();
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
		std::cerr << "Failed to find " << this->_query.getRequestTarget() << std::endl;
		return (SERV_ERROR);
	}
	this->_ressource_status = EXISTS;
	if (S_ISDIR(file_stat.st_mode))
	{
		if (stat((this->_ressource + std::string("index.html")).c_str(), &dir_stat) == 0)
		{
			this->_ressource += "index.html";
			this->setRessourceStatus();
		}
		else
			this->_ressource_status |= IS_DIR;
	}
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
		this->_query.setMethod(CGI_RUN);
	}
	this->_content_len = file_stat.st_size;
	return (this->_ressource_status);
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

void		Query::setHeader()
{
	this->_header = "HTTP/1.0 " + std::to_string(this->_err_code) + " OK\r\n";
	this->_header += "Server: Apache/1.3.29 (Unix)\r\n";
	this->_header += "Content-Type: " + this->getRessourceTypeStr() + "\r\n";
	this->_header += "Content-Length: " + std::to_string(this->_content_len) + "\r\n\r\n";
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
	//TODO add epoll wait for client Fd ?
	bytes_read = read(fd, buffer, sizeof(buffer));
	if (bytes_read <= 0)
	{
		//set err
		return (SERV_ERROR);
	}
	while (bytes_read > 0)
	{
		//TODO add epoll wait for client Fd ?
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