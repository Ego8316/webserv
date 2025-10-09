/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:30 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/09 22:00:50 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Query.hpp"

Query::Query(Config *config, Client *client, std::map<std::string, Cookie *> *all_cookies)
	:	_config(config),
		_client(client),
		_all_cookies(all_cookies)
{
	this->_query = new Request(all_cookies);
	this->_content_len = 0;
	this->_err_code = HTTP_OK;
	this->_resource_status = FILE_NOT_FOUND;
	this->_content_type = FTYPE_PLAIN;
}

Query::Query(const Query &other)
	:	_config(other._config),
		_client(other._client),
		_all_cookies(other._all_cookies)
{
	_err_code = other._err_code;
	_query = other._query ? new Request(*other._query) : nullptr;
	_content_len = other._content_len;
	_resource = other._resource;
	_header = other._header;
	_resource_status = other._resource_status;
	_content_type = other._content_type;
}

Query &Query::operator=(const Query &other)
{
	if (this != &other)
	{
		_config = other._config;
		_client = other._client;
		_all_cookies = other._all_cookies;
		_err_code = other._err_code;

		delete _query;
		_query = other._query ? new Request(*other._query) : nullptr;

		_content_len = other._content_len;
		_resource = other._resource;
		_header = other._header;
		_resource_status = other._resource_status;
		_content_type = other._content_type;
	}
	return (*this);
}

Query::~Query(void)
{
	delete this->_query;
}

int		Query::queryRespond(void)
{
	this->readRequest();
	std::cout << "REQUEST = " << std::endl;
	std::cout << this->_request_str << std::endl;
	if (this->_request_str.length() == 0)
	{
		std::cerr << "queryRespond: Could not retrieve query" << std::endl;
		return (SERV_ERROR);
	}
	std::cout << _request_str << std::endl;
	this->_query->parseRequest(this->_request_str);
	if (this->_query->getError() != NONE)
	{
		if (this->_query->getError() == UNSUPPORTED_METHOD)
			this->_err_code = NOT_IMPLEMENTED;
		else
			this->_err_code = HTTP_BAD_REQUEST;
		return (this->queryError());
	}
	this->_query_cookies = this->_query->getQueryCookies();
	if (this->_query_cookies.size() == 0)
		std::cerr << "Cookie failed" << std::endl;
	if (this->setResource() == SERV_ERROR)
		return (SERV_ERROR);
	this->screenErrors();
	if (this->_err_code >= HTTP_BAD_REQUEST)
		return (this->queryError());
	if (this->_http_redirect.length() != 0)
		return (queryRedirect());
	if (this->_resource_status & IS_DIR)
		return (this->queryListDir());
	//TODO add some funcs ?
	return ((this->*_queryExecute[std::min(static_cast<int>(this->_query->getMethod()), (int)ERROR)])());
}

int		Query::screenErrors()
{
	if (this->_err_code != 200)
		return (SERV_ERROR);
	if (!(this->_resource_status & PERM_ROK) && this->_query->getMethod() == GET)
	{
		this->_err_code = HTTP_FORBIDDEN;
		return (SERV_ERROR);
	}
	else if (!(this->_resource_status & PERM_WOK)
		&& (this->_query->getMethod() == POST || this->_query->getMethod() == DELETE))
	{
		this->_err_code = HTTP_FORBIDDEN;
		return (SERV_ERROR);
	}
	else if (!(this->_resource_status & PERM_XOK) && this->_query->getMethod() == CGI_RUN)
	{
		this->_err_code = HTTP_FORBIDDEN;
		return (SERV_ERROR);
	}
	if ((this->_resource_status & IS_DIR))
	{
		this->_err_code = HTTP_BAD_REQUEST;
		return (SERV_ERROR);
	}
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
		this->_request_str += std::string(buffer).substr(0, bytes_read);
	}
	return (0);
}

int		Query::queryGet(void)
{
	this->setHeader();
	if (access(this->_resource.c_str(), R_OK) == 0)
	{
		if (this->sendHeader() == SERV_ERROR)
		{
			std::cerr << "Failed to send header" << std::endl;
			return (SERV_ERROR);
		}
		this->streamFile(this->_resource);
	}
	else
		std::cerr << "Could not access resource : >" << this->_resource << "<" << std::endl;
	std::cout << "Query answered with code " << this->_err_code << std::endl;
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
	// std::string	defaultErrorPage = getDefaultErrorPage(_statusCode);

	// _content_len = defaultErrorPage.size();
	// this->setHeader();
	// this->sendHeader();
	this->streamFile(getDefaultErrorPage(_err_code));
	return (0);
}

int		Query::queryRedirect()
{
	//TODO some shit here
	return (0);
}

int		Query::queryListDir()
{
	std::string		ret_body;
	DIR				*dir;
	struct dirent	*dir_ent;
	
	dir = opendir(this->_resource.c_str());
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
	if (this->sendHeader() == SERV_ERROR)
		std::cerr << "SetHeader failed" << std::endl;
	return (this->_client->socketWrite(ret_body.c_str(), ret_body.length()));
}

int		Query::setResource()
{
	this->_resource = this->_query->getRequestTarget();
	this->checkRedirections();
	if (this->_http_redirect.length())
		return (0);
	if (this->findResource() == SERV_ERROR)
	{
		std::cerr << "Resource could not be found" << std::endl;
		return (SERV_ERROR);
	}
	if (this->_err_code >= HTTP_BAD_REQUEST) //TODO
		return (SERV_ERROR);
	if (this->setResourceStatus() == SERV_ERROR)
	{
		std::cerr << "Resource status error " << std::endl;
		return (SERV_ERROR);
	}
	return (0);
}

void	Query::checkRedirections()
{
	std::string		raw_path_requested;
	std::string		raw_redir_key;

	raw_path_requested = this->_resource;
	if (utils::startsWith(raw_path_requested, "https://"))
		raw_path_requested.erase(0, 9);
	if (utils::startsWith(raw_path_requested, "http://"))
		raw_path_requested.erase(0, 8);
	if (utils::startsWith(raw_path_requested, "www"))
		raw_path_requested.erase(0, raw_path_requested.find("/"));
	for (std::map<std::string, Redirection>::iterator it = this->_config->http_redir.begin();
		it != this->_config->http_redir.end(); ++it)
	{
		raw_redir_key = it->first;
		this->_err_code = REDIRECT_MOVE;
	}
}

int		Query::findResource()
{
	struct stat file_stat;
	struct stat dir_stat;
	
	this->_resource = this->_config->server_home + this->_resource;
	if (stat(this->_resource.c_str(), &file_stat) == -1)
	{
		if (errno == EACCES)
			this->_err_code = HTTP_FORBIDDEN;
		else
			this->_err_code = HTTP_NOT_FOUND;
		std::cerr << "Resource stat failed: " << strerror(errno) << std::endl;
		std::cerr << "Failed to find " << this->_resource << std::endl;
		return (SERV_ERROR);
	}
	else
	{
		if (S_ISDIR(file_stat.st_mode))
		{
			if (stat((this->_resource + this->_config->default_page).c_str(), &dir_stat) == 0)
			{
				this->_resource += this->_config->default_page;
				return (0);
			}
			else if (this->_config->enable_listdir == false)
			{
				std::cerr << "Resource is a directory " << this->_resource << std::endl;
				return (SERV_ERROR);
			}
		}
	}
	return (0);
}

int		Query::setResourceStatus()
{
	struct stat file_stat;

	if (stat(this->_resource.c_str(), &file_stat) == SERV_ERROR)
	{
		if (errno == EACCES)
		{
			this->_err_code = HTTP_FORBIDDEN;
			this->_resource_status = PERM_ISSUE;
		}
		else
		{
			this->_err_code = HTTP_NOT_FOUND;
			this->_resource_status = FILE_NOT_FOUND;
		}
		std::cerr << "Resource stat failed: " << strerror(errno) << std::endl;
		std::cerr << "Failed to find " << this->_resource << std::endl;
		return (SERV_ERROR);
	}
	this->_resource_status = EXISTS;
	if (S_ISDIR(file_stat.st_mode))
		this->_resource_status = static_cast<FileStatus>(this->_resource_status | IS_DIR);
	if (file_stat.st_mode & S_IRUSR)
		this->_resource_status = static_cast<FileStatus>(this->_resource_status | PERM_ROK);
	if (file_stat.st_mode & S_IWUSR)
		this->_resource_status = static_cast<FileStatus>(this->_resource_status | PERM_WOK);
	if (file_stat.st_mode & S_IXUSR)
		this->_resource_status = static_cast<FileStatus>(this->_resource_status | PERM_XOK);
	if (this->_resource_status <= IS_DIR)
		this->_resource_status = static_cast<FileStatus>(this->_resource_status | PERM_ISSUE);
	if (utils::endsWith(this->_resource, ".py"))
	{
		this->_resource_status = static_cast<FileStatus>(this->_resource_status | IS_CGI);
		this->_query->setMethod(CGI_RUN);
		this->_content_type = FTYPE_CGI_PY;
	}
	else if (utils::endsWith(this->_resource, ".php"))
	{
		this->_resource_status = static_cast<FileStatus>(this->_resource_status | IS_CGI);
		this->_query->setMethod(CGI_RUN);
		this->_content_type = FTYPE_CGI_PHP;
	}
	else if (utils::endsWith(this->_resource, ".html"))
		this->_content_type = FTYPE_HTML;
	else if (utils::endsWith(this->_resource, ".jpeg"))
		this->_content_type = FTYPE_JPEG;
	else if (utils::endsWith(this->_resource, ".png"))
		this->_content_type = FTYPE_PNG;
	else if (utils::endsWith(this->_resource, ".txt") || this->_resource.find(".") == std::string::npos)
		this->_content_type = FTYPE_PLAIN;
	this->_content_len = file_stat.st_size;
	//TODO check Accept ici
	return (0);
}

std::string	Query::getResourceTypeStr(void)
{
	if (this->_content_type == FTYPE_HTML)
		return ("text/html");
	if (this->_content_type == FTYPE_PLAIN)
		return ("text/plain");
	if (this->_content_type == FTYPE_JPEG)
		return ("image/jpeg");
	if (this->_content_type == FTYPE_PNG)
		return ("image/png");
	return ("");
}

std::string	Query::getResourceTypeExtenssion()
{
	if (this->_content_type == FTYPE_HTML)
		return (".html");
	if (this->_content_type == FTYPE_PLAIN)
		return ("");
	if (this->_content_type == FTYPE_JPEG)
		return (".jpeg");
	if (this->_content_type == FTYPE_PNG)
		return (".png");
	if (this->_content_type == FTYPE_CGI_PY)
		return (".py");
	if (this->_content_type == FTYPE_CGI_PHP)
		return (".php");
	
	return ("");
}

void		Query::setHeader()
{
	this->_header = "HTTP/1.0 "
		+ utils::toString(this->_err_code)
		+ " " + httpStatusToStr(this->_err_code) + "\r\n";
	this->_header += "Server: Apache/1.3.29 (Unix)\r\n";
	for (unsigned int i = 0; i < this->_query_cookies.size(); ++i)
		this->_header += this->_query_cookies[i]->genHeader() + "\r\n";
	this->_header += "Connection: close \r\n";
	this->_header += "Content-Type: " + this->getResourceTypeStr() + "\r\n";
	this->_header += "Content-Length: " + utils::toString(this->_content_len) + "\r\n\r\n";
}

int		Query::sendHeader(void)
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
		if (this->_client->socketWrite(buffer, bytes_read) == SERV_ERROR)
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
		case HTTP_OK:							return "OK";
		case HTTP_BAD_REQUEST:					return "Bad Request";
		case HTTP_FORBIDDEN:						return "Forbidden";
		case HTTP_NOT_FOUND:						return "Not Found";
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
		case HTTP_BAD_REQUEST:					return ERROR_PAGE_400;
		case HTTP_FORBIDDEN:						return ERROR_PAGE_403;
		case HTTP_NOT_FOUND:						return ERROR_PAGE_404;
		case INTERNAL_SERVER_ERROR:			return ERROR_PAGE_500;
		case NOT_IMPLEMENTED:				return ERROR_PAGE_501;
		case HTTP_VERSION_NOT_SUPPORTED:	return ERROR_PAGE_505;
		default:							return "<html><body><h1>Unknown Error</h1></body></html>";
	}
}

const Query::queryMethod	Query::_queryExecute[_method_num] = {
	&Query::queryGet,
	&Query::queryPost, 
	&Query::queryDelete,
	&Query::queryCGIRun,
	&Query::queryError
};
