/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:33:19 by ego               #+#    #+#             */
/*   Updated: 2025/10/10 15:05:38 by ego              ###   ########.fr       */
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

Response	RequestHandler::handle(const Request &req, const Config &config, std::vector<Cookie *> cookies)
{
	(void)cookies;
	if (req.getError() == UNSUPPORTED_METHOD)
		return (_handleError(HTTP_NOT_IMPLEMENTED, config));
	else if (req.getError() >= INVALID_REQUEST_LINE)
		return _handleError(HTTP_BAD_REQUEST, config);

	Resource	res;
	res.build(req.getRequestTarget(), config);

	if (!res.exists())
		return (_handleError(HTTP_NOT_FOUND, config));
	if (res.isForbidden())
		return (_handleError(HTTP_FORBIDDEN, config));
	if (res.isDirectory())
		return (_handleListDir(req, config, res));
	if (res.isCGI())
		return (_handleCGI(req, config, res));

	switch (req.getMethod())
	{
		case GET:		return _handleGet(req, config, res);
		case POST:		return _handlePost(req, config, res);
		case DELETE:	return _handleDelete(req, config, res);
		default:		return _handleError(HTTP_NOT_IMPLEMENTED, config);
	}
}

Response	RequestHandler::_handleGet(const Request &req, const Config &config, const Resource &res)
{
	Response			response;
	std::ifstream		file;
	std::ostringstream	buffer;
	std::string			content;

	(void)req;
	file.open(res.getPath().c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return (_handleError(HTTP_INTERNAL_SERVER_ERROR, config));
	buffer << file.rdbuf();
	content = buffer.str();
	file.close();

	response.setStatus(HTTP_OK);
	response.setBody(content);
	response.setContentLength(content.size());
	response.setContentType(res.getMimeType());
	response.buildHeader();
	return (response);
}

Response	RequestHandler::_handlePost(const Request &req, const Config &config, const Resource &res)
{
	Response	response;

	(void)req;
	(void)config;
	(void)res;
	
	return (response);
}

Response	RequestHandler::_handleDelete(const Request &req, const Config &config, const Resource &res)
{
	Response	response;

	(void)req;
	(void)config;
	(void)res;
	
	return (response);
}

Response	RequestHandler::_handleCGI(const Request &req, const Config &config, const Resource &res)
{
	Response	response;

	(void)req;
	(void)config;
	(void)res;
	
	return (response);
}

Response	RequestHandler::_handleRedirect(const Request &req, const Config &config, const Resource &res)
{
	Response		response;
	std::string		raw_path_requested;
	std::string		raw_redir_key;

	(void)req;
	(void)config;
	raw_path_requested = res.getPath();
	if (utils::startsWith(raw_path_requested, "https://"))
		raw_path_requested.erase(0, 9);
	if (utils::startsWith(raw_path_requested, "http://"))
		raw_path_requested.erase(0, 8);
	if (utils::startsWith(raw_path_requested, "www"))
		raw_path_requested.erase(0, raw_path_requested.find("/"));
	/*for (std::map<std::string, Redirection>::iterator it = config.http_redir.begin(); it != config.http_redir.end(); ++it)
	{
		raw_redir_key = it->first;
		response.setStatus(HTTP_OK); // mettre le bon statut
	}*/
	return (response);
}

Response	RequestHandler::_handleListDir(const Request &req, const Config &config, const Resource &res)
{
	Response		response;
	std::string		response_body;
	DIR				*dir;
	struct dirent	*dir_ent;
	
	(void)req;
	dir = opendir(res.getPath().c_str());
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
