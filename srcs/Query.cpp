/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:30 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/25 22:19:42 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Query.hpp"

Query::Query() {}

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

int		Query::queryRespond()
{
	if (this->_query_str.length() == 0)
	{
		std::cerr << "queryRespond: Could not retrieve query" << std::endl;
		return (-1);
	}
	this->_query = Request(this->_query_str);
	(this->*_queryExecute[std::max(static_cast<int>(this->_query.getMethod()), DELETE + 1)])();
	return (0);
}

int		Query::setRessourceStatus()
{
	struct stat file_stat;
	struct stat dir_stat;

	if (stat(this->_query.getRequestTarget().c_str(), &file_stat) == -1)
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
		return (-1);
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
		this->_cgi_request = true;
	}
	else
		this->_cgi_request = false;
	return (this->_ressource_status);
}

int		Query::queryGet()
{
	struct stat file_stat;

	if (!(this->_ressource_status & PERM_ROK))
	{
		this->_err_code = 403;
		return (-1);
	}
	if ((this->_ressource_status & IS_DIR))
	{
		if
		{	
			this->_err_code = 200;
			return (-1);
		}
	}
	if (access(this->_ressource.c_str(), R_OK) == 0)
	{
		this->readFile(this->_ressource, );
	}
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