/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vviterbo <vviterbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:33:19 by ego               #+#    #+#             */
/*   Updated: 2025/12/08 15:00:57 by vviterbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

static std::string	getAllow(const Location *loc)
{
	if (loc == NULL)
		return ("GET");
	std::string	methods;
	if (loc->methods & GET)    methods += "GET, ";
	if (loc->methods & POST)   methods += "POST, ";
	if (loc->methods & DELETE) methods += "DELETE, ";
	methods.erase(methods.size() - 2);
	return (methods);
}

/**
 * @brief Default constructor (unused because all methods are static).
 */
RequestHandler::RequestHandler(void)
{
	return ;
}

/**
 * @brief Destructor.
 */
RequestHandler::~RequestHandler(void)
{
	return ;
}

/**
 * @brief Entry point that routes a parsed request to the proper handler.
 *
 * @param response Response object to fill.
 * @param request Parsed request.
 * @param config Server configuration.
 */
void	RequestHandler::handle(Response *response, const Request &request, const ServerConfig &config)
{
	if (request.getContentLength() > config.client_max_body_size)
		return (handleError(response, HTTP_CONTENT_TOO_LARGE, config));
	if (request.getError())
		return (handleError(response, HTTP_BAD_REQUEST, config));
	if (request.getMethod() == UNKNOWN)
		return (handleError(response, HTTP_NOT_IMPLEMENTED, config));
	if (request.getVersion() != "HTTP/1.1" && request.getVersion() != "HTTP/1.0")
		return (handleError(response, HTTP_VERSION_NOT_SUPPORTED, config));
	else if (request.getVersion() == "HTTP/1.1")
		response->setVersion("HTTP/1.1");
	else
		response->setVersion("HTTP/1.0");
	Resource	resource;
	resource.build(request, config);

	if (resource.isRedirect())
		return (_handleRedirect(response, resource));
	if (!resource.methodAllowed())
	{
		response->setHeaders("Allow", getAllow(resource.getLocation()));
		return (handleError(response, HTTP_METHOD_NOT_ALLOWED, config));
	}
	if (resource.isForbidden() || resource.isHidden())
		return (handleError(response, HTTP_FORBIDDEN, config));
	if (!resource.exists() && request.getMethod() != POST)
		return (handleError(response, HTTP_NOT_FOUND, config));
	if (resource.getStatus() & ACCEPT_ERROR)
		return (handleError(response, HTTP_NOT_ACCEPTABLE, config));
	if (resource.isCGI())
		return (_handleCGI(response, config, resource));
	switch (request.getMethod())
	{
		case GET:		return _handleGet(response, config, resource);
		case POST:		return _handlePost(response, request, config, resource);
		case DELETE:	return _handleDelete(response, config, resource);
		default:		return handleError(response, HTTP_NOT_IMPLEMENTED, config);
	}
}

/**
 * @brief Handles GET requests by serving files or emitting directory errors.
 *
 * @param response Response to populate.
 * @param config Server configuration.
 * @param resource Resolved resource.
 */
void	RequestHandler::_handleGet(Response *response, const ServerConfig &config, const Resource &resource)
{
	int			fd;
	ssize_t		size;

	if (!resource.isReadable())
		return (handleError(response, HTTP_FORBIDDEN, config));

	if (resource.isDirectory())
	{
		if (resource.autoindex())
			return (_handleListDir(response, config, resource));
		return (handleError(response, HTTP_FORBIDDEN, config));
	}

	if ((fd = open(resource.getPath().c_str(), O_RDONLY)) == -1)
		return (_handleErrno(response, config));
	if ((size = utils::getFileSize(resource.getPath())) == -1)
		return (_handleErrno(response, config));
	
	response->setStatus(HTTP_OK);
	response->setFd(fd);
	response->setContentLength(size);
	response->setContentType(utils::contentTypeToStr(resource.getType()));
	response->build();
}

/**
 * @brief Handles POST uploads by writing the body to the resolved path.
 *
 * @param response Response to populate.
 * @param request Client request.
 * @param config Server configuration.
 * @param resource Resolved resource.
 */
void	RequestHandler::_handlePost(Response *response, const Request &request, const ServerConfig &config, const Resource &resource)
{
	std::ofstream	outfile;
	bool			existed;

	if (resource.isDirectory())
		return (handleError(response, HTTP_CONFLICT, config));
	std::ifstream	check(resource.getPath().c_str());
	existed = check.good();
	check.close();

	outfile.open(resource.getPath().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	if (!outfile.is_open())
		return (_handleErrno(response, config));
	if (!request.getRawBody().empty())
		outfile << request.getRawBody();
	outfile.close();
	if (existed)
	{
		response->setStatus(HTTP_NO_CONTENT);
		response->setBody("");
	}
	else
	{
		response->setStatus(HTTP_CREATED);
		response->setBody(POST_PAGE);
		response->setContentType("text/html");
		response->setHeaders("Location", resource.getPath());
	}
	response->setContentLength(response->getBody().size());
	response->build();
}

/**
 * @brief Handles DELETE requests, mapping errno to HTTP status codes.
 *
 * @param response Response to populate.
 * @param config Server configuration.
 * @param resource Resolved resource.
 */
void	RequestHandler::_handleDelete(Response *response, const ServerConfig &config, const Resource &resource)
{
	if (resource.isDirectory() && !utils::endsWith(resource.getPath(), "/"))
		return (handleError(response, HTTP_CONFLICT, config));
	if (std::remove(resource.getPath().c_str()) != 0)
		return (_handleErrno(response, config));
	response->setStatus(HTTP_NO_CONTENT);
	response->setBody("");
	response->setContentLength(0);
	response->build();
}

/**
 * @brief Initializes CGI handling when the resource is executable.
 *
 * @param response Response to populate with CGI handler.
 * @param request Parsed request.
 * @param config Server configuration.
 * @param resource Resolved resource.
 */
void	RequestHandler::_handleCGI(Response *response, const ServerConfig &config, const Resource &resource)
{
	if (!resource.isExecutable() || resource.isDirectory())
		return (handleError(response, HTTP_FORBIDDEN, config));
	if (response->getCGI() == NULL)
		response->setCGI(new CGI(resource.getPath()));
}

/**
 * @brief Sends an HTTP redirect response using the resource redirection info.
 *
 * @param response Response to populate.
 * @param resource Redirect resource.
 */
void	RequestHandler::_handleRedirect(Response *response, const Resource &resource)
{
	std::string		response_body;

	response->setStatus(resource.getRedirectCode());
	response->setHeaders("Location", resource.getPath());
	response->setContentType("text/html");
	response->setContentLength(0);
	response->build();
}

/**
 * @brief Builds a simple directory listing response body.
 *
 * @param response Response to populate.
 * @param config Server configuration.
 * @param resource Resolved directory resource.
 */
void	RequestHandler::_handleListDir(Response *response, const ServerConfig &config, const Resource &resource)
{
	std::string		response_body;
	DIR				*dir;
	struct dirent	*dir_ent;
	
	dir = opendir(resource.getPath().c_str());
	if (dir == NULL)
		return (_handleErrno(response, config));
	response_body = LISTDIR_HEADER;
	dir_ent = readdir(dir);
	while (dir_ent != NULL)
	{
		response_body += LISTDIR_PREFIX + std::string(dir_ent->d_name) + LISTDIR_SUFFIX;
		dir_ent = readdir(dir);
	}
	response_body += LISTDIR_ENDING;
	closedir(dir);
	response->setStatus(HTTP_OK);
	response->setBody(response_body);
	response->setContentLength(response_body.length());
	response->setContentType("text/html");
	response->build();
}

/**
 * @brief Builds an error response, preferring configured custom pages.
 *
 * @param response Response to populate.
 * @param code HTTP status code to send.
 * @param config Server configuration for custom pages.
 */
void	RequestHandler::handleError(Response *response, HttpStatus code, const ServerConfig &config)
{
	std::string	error_page_path;
	int			fd;
	ssize_t		size;

	response->setStatus(code);
	response->setContentType("text/html");
	if (utils::mapHasEntry(config.error_pages, (int)code))
	{
		error_page_path = config.root + config.error_pages.at(code);
		if ((fd = open(error_page_path.c_str(), O_RDONLY)) >= 0)
		{
			if ((size = utils::getFileSize(error_page_path)) != -1)
			{
				response->setFd(fd);
				response->setContentLength(size);
				response->build();
				return ;
			}
			close(fd);
		}
	}
	response->setBody(Response::getDefaultErrorPage(code));
	response->setContentLength(response->getBody().size());
	response->build();
}

void	RequestHandler::_handleErrno(Response *response, const ServerConfig &config)
{
	if (errno == EACCES || errno == EPERM)
		return (handleError(response, HTTP_FORBIDDEN, config));
	if (errno == ENOENT)
		return (handleError(response, HTTP_NOT_FOUND, config));
	if (errno == ENOTEMPTY)
		return (handleError(response, HTTP_CONFLICT, config));
	return (handleError(response, HTTP_INTERNAL_SERVER_ERROR, config));
}
