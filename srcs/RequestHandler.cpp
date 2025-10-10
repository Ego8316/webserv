/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:33:19 by ego               #+#    #+#             */
/*   Updated: 2025/10/10 18:40:49 by victorviter      ###   ########.fr       */
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

Response	RequestHandler::handle(const Request &request, const Config &config, std::vector<Cookie *> cookies)
{
	(void)cookies;
	if (request.getError() == UNSUPPORTED_METHOD)
		return (_handleError(HTTP_NOT_IMPLEMENTED, config));
	else if (request.getError() >= INVALID_REQUEST_LINE)
		return _handleError(HTTP_BAD_REQUEST, config);

	Resource	resource;
	resource.build(request.getRequestTarget(), config);

	if (resource.isRedirect())
		return (_handleRedirect(request, config, resource));
	if (!resource.exists()) //TODO -> @Hugo faut corriger ca du coup par ex. pour un post c'est OK
	{
		std::cout << "Resource not found" << std::endl;
		return (_handleError(HTTP_NOT_FOUND, config));
	}
	if (resource.isForbidden()) //TODO -> pareil
		return (_handleError(HTTP_FORBIDDEN, config));
	if (resource.isDirectory()) //TODO -> pareil pour delete
		return (_handleListDir(request, config, resource));
	if (resource.isCGI())
		return (_handleCGI(request, config, resource));

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
	Response			response;
	std::ifstream		file;
	std::ostringstream	buffer;
	std::string			content;

	(void)request;
	file.open(resource.getPath().c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return (_handleError(HTTP_INTERNAL_SERVER_ERROR, config));
	buffer << file.rdbuf();
	content = buffer.str();
	file.close();

	response.setStatus(HTTP_OK);
	response.setBody(content);
	response.setContentLength(content.size());
	response.setContentType(resource.getMimeType());
	response.buildHeader();
	return (response);
}

Response	RequestHandler::_handlePost(const Request &request, const Config &config, const Resource &resource)
{
	Response	response;

	(void)request;
	(void)config;
	(void)resource;
	
	return (response);
}

Response	RequestHandler::_handleDelete(const Request &request, const Config &config, const Resource &resource)
{
	Response	response;

	(void)request;
	(void)config;
	(void)resource;
	
	return (response);
}

Response	RequestHandler::_handleCGI(const Request &request, const Config &config, const Resource &resource)
{
	Response	response;

	(void)request;
	(void)config;
	(void)resource;
	
	return (response);
}

Response	RequestHandler::_handleRedirect(const Request &request, const Config &config, const Resource &resource)
{
	Response		response;
	std::string		response_body;

	(void)request;
	(void)config;
	std::cout << "HANDLING REDIRECT" << std::endl;
	response.setStatus(static_cast<HttpStatus>(resource.getStatus()));
	response.setHeaders("Location", resource.getPath());
	response.setContentType("text/html");
	response.setContentLength(0);
	response.buildHeader();
	return (response);
}

Response	RequestHandler::_handleListDir(const Request &request, const Config &config, const Resource &resource)
{
	Response	response;
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
	response.buildHeader();
	return (response);
}

Response	RequestHandler::_handleError(HttpStatus code, const Config &config)
{
	Response	response;
	std::string	content;

	if (utils::mapHasEntry(config.default_error_pages, (int)code))
	{
		std::ifstream	file(config.default_error_pages.at((int)code).c_str());
		if (file)
			content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	}
	else
		content = Response::getDefaultErrorPage(code);
	response.setStatus(code);
	response.setBody(content);
	response.setContentLength(content.size());
	response.setContentType("text/html");
	response.buildHeader();
	return (response);
}
