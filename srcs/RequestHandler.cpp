/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:33:19 by ego               #+#    #+#             */
/*   Updated: 2025/10/23 04:18:15 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

RequestHandler::RequestHandler(void)
{
	return ;
}

RequestHandler::RequestHandler(const RequestHandler &other)
{
	(void)other;
	return ;
}

RequestHandler	&RequestHandler::operator=(const RequestHandler &other)
{
	(void)other;
	return (*this);
}

RequestHandler::~RequestHandler(void)
{
	return ;
}

Response	RequestHandler::handle(const Request &request, const Config &config, const Cookie &cookies)
{
	(void)cookies;
	if (request.getError())
	{
		// if (request.getContentLength() > config.max_body_size)
			// return (_handleError(HTTP_CONTENT_TOO_LARGE, config));
		return (_handleError(HTTP_BAD_REQUEST, config));
	}
	if (request.getMethod() == UNKNOWN)
		return (_handleError(HTTP_NOT_IMPLEMENTED, config));
	// TODO : BIEN PENSER A CHANGER AVANT DE RENDRE
	// if (request.getVersion() != "HTTP/1.1" || request.getVersion() != "HTTP/1.0")
	// 	return (_handleError(HTTP_VERSION_NOT_SUPPORTED, config));

	Resource	resource;
	resource.build(request, config);

	if (resource.isRedirect())
		return (_handleRedirect(request, config, resource));
	if (!resource.exists() && request.getMethod() != POST)
	{
		std::cerr << "Resource not found" << std::endl;
		return (_handleError(HTTP_NOT_FOUND, config));
	}
	if (resource.isForbidden())
		return (_handleError(HTTP_FORBIDDEN, config));
	if (resource.getStatus() & ACCEPT_ERROR)
		return (_handleError(HTTP_BAD_REQUEST, config));
	if (resource.isCGI())
		return (_handleCGI());
	switch (request.getMethod())
	{
		case GET:		return _handleGet(request, config, resource);
		case POST:		return _handlePost(request, config, resource);
		case DELETE:	return _handleDelete(request, config, resource);
		default:		return _handleError(HTTP_NOT_IMPLEMENTED, config);
	}
}

Response	RequestHandler::_handleGet(const Request &request, const Config &config, const Resource &resource)
{
	Response	response;
	int			fd;
	ssize_t		size;

	if (!resource.isReadable())
		return (_handleError(HTTP_FORBIDDEN, config));

	if (resource.isDirectory())
	{
		if (!utils::endsWith(resource.getPath(), "/"))
			return (_handleError(HTTP_REDIRECT_PERM, config));
		if (config.enable_listdir)
			return (_handleListDir(request, config, resource));
		return (_handleError(HTTP_FORBIDDEN, config));
	}
	
	if ((fd = open(resource.getPath().c_str(), O_RDONLY)) == -1)
		return (_handleError(HTTP_INTERNAL_SERVER_ERROR, config));
	if ((size = utils::getFileSize(resource.getPath())) == -1)
		return (close(fd), _handleError(HTTP_INTERNAL_SERVER_ERROR, config));
	
	response.setStatus(HTTP_OK);
	response.setFd(fd);
	response.setContentLength(size);
	response.setContentType(utils::contentTypeToStr(resource.getType()));
	response.build();
	return (response);
}

Response	RequestHandler::_handlePost(const Request &request, const Config &config, const Resource &resource)
{
	Response		response = Response();
	std::ofstream	outfile;
	bool			existed;

	(void)request;
	if (request.getRawBody().empty() || resource.isDirectory())
		return (_handleError(HTTP_BAD_REQUEST, config));

	std::ifstream	check(resource.getPath().c_str());
	existed = check.good();

	outfile.open(resource.getPath().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	if (!outfile.is_open())
		return (_handleError(HTTP_INTERNAL_SERVER_ERROR, config));
	outfile << request.getRawBody();
	outfile.close();

	if (existed)
		response.setStatus(HTTP_NO_CONTENT);
	else
	{
		response.setStatus(HTTP_CREATED);
		response.setHeaders("Location", resource.getPath());
	}
	response.setBody(POST_PAGE);
	response.setContentType("text/html");
	response.setContentLength(response.getBody().size());
	response.build();
	return (response);
}

Response	RequestHandler::_handleDelete(const Request &request, const Config &config, const Resource &resource)
{
	Response	response;

	(void)request;
	if (resource.isDirectory() && !utils::endsWith(resource.getPath(), "/"))
		return (_handleError(HTTP_CONFLICT, config));
	if (std::remove(resource.getPath().c_str()) != 0)
	{
		if (errno == EACCES || errno == EPERM)
			return (_handleError(HTTP_FORBIDDEN, config));
		if (errno == ENOENT)
			return (_handleError(HTTP_NOT_FOUND, config));
		return (_handleError(HTTP_INTERNAL_SERVER_ERROR, config));
	}
	response.setStatus(HTTP_NO_CONTENT);
	response.setBody("");
	response.setContentLength(0);
	response.build();
	return (response);
}

Response	RequestHandler::_handleCGI()
{
	Response	response;

	//TODO check for errors on Ressource
	if (response.getCGI() == NULL)
		response.setCGI(new CGI);
	return (response);
}

Response	RequestHandler::_handleRedirect(const Request &request, const Config &config, const Resource &resource)
{
	Response		response;
	std::string		response_body;

	(void)request;
	(void)config;
	response.setStatus(static_cast<HttpStatus>(resource.getStatus()));
	response.setHeaders("Location", resource.getPath());
	response.setContentType("text/html");
	response.setContentLength(0);
	response.build();
	return (response);
}

Response	RequestHandler::_handleListDir(const Request &request, const Config &config, const Resource &resource)
{
	Response		response;
	std::string		response_body;
	DIR				*dir;
	struct dirent	*dir_ent;
	
	(void)request;
	dir = opendir(resource.getPath().c_str());
	if (dir == NULL)
	{
		std::cerr << "Cannot open directory\n" << std::endl;
		return (_handleError(HTTP_INTERNAL_SERVER_ERROR, config));
	}
	response_body = LISTDIR_HEADER;
	dir_ent = readdir(dir);
	while (dir_ent != NULL)
	{
		response_body += LISTDIR_PREFIX + std::string(dir_ent->d_name) + LISTDIR_SUFFIX;
		dir_ent = readdir(dir);
	}
	response_body += LISTDIR_ENDING;
	closedir(dir);
	response.setStatus(HTTP_OK);
	response.setBody(response_body);
	response.setContentLength(response_body.length());
	response.setContentType("text/html");
	response.build();
	return (response);
}

Response	RequestHandler::_handleError(HttpStatus code, const Config &config)
{
	Response	response;
	std::string	error_page_path;
	int			fd;
	ssize_t		size;

	response.setStatus(code);
	response.setContentType("text/html");

	if (utils::mapHasEntry(config.default_error_pages, (int)code))
	{
		error_page_path = config.default_error_pages.at(code);
		if ((fd = open(error_page_path.c_str(), O_RDONLY)) >= 0)
		{
			if ((size = utils::getFileSize(error_page_path)) != -1)
			{
				response.setFd(fd);
				response.setContentLength(size);
				response.build();
				return (response);
			}
			close(fd);
		}
	}

	response.setBody(Response::getDefaultErrorPage(code));
	response.setContentLength(response.getBody().size());
	response.build();
	return (response);
}
