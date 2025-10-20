/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:33:19 by ego               #+#    #+#             */
/*   Updated: 2025/10/20 23:42:54 by victorviter      ###   ########.fr       */
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
		return _handleError(HTTP_BAD_REQUEST, config);
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
		return (_handleCGI(request, config, resource, cookies));
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

	file.open(resource.getPath().c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return (_handleError(HTTP_INTERNAL_SERVER_ERROR, config));
	buffer << file.rdbuf();
	file.close();

	response.setStatus(HTTP_OK);
	response.setBody(buffer.str());
	response.setContentLength(buffer.str().size());
	response.setContentType(utils::contentTypeToStr(resource.getType()));
	response.buildHeader();
	return (response);
}

Response	RequestHandler::_handlePost(const Request &request, const Config &config, const Resource &resource)
{
	Response		response;
	std::ofstream	outfile;

	(void)request;
	if (request.getRawBody().empty() || resource.isDirectory())
		return (_handleError(HTTP_BAD_REQUEST, config));

	outfile.open(resource.getPath().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	if (!outfile.is_open())
		return (_handleError(HTTP_INTERNAL_SERVER_ERROR, config));
	outfile << request.getRawBody();
	outfile.close();

	response.setStatus(HTTP_CREATED);
	response.setBody("Sahtek frerot");
	response.setContentType("text/html");
	response.setContentLength(response.getBody().size());
	response.buildHeader();
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
	response.buildHeader();
	return (response);
}

Response	RequestHandler::_handleCGI(const Request &request, const Config &config, const Resource &resource, const Cookie &cookies)
{
	Response	response;

	(void)request;
	(void)config;
	(void)resource;
	(void)cookies;
	response._is_cgi = true;
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
	response.buildHeader();
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
