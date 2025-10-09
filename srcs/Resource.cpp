/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 22:18:46 by ego               #+#    #+#             */
/*   Updated: 2025/10/09 02:12:45 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Resource.hpp"

/**
 * @brief Default constructor. Initializes status code to 404 NOT FOUND and
 * and type to PLAIN.
 */
Resource::Resource(void)
	:	_path(""),
		_status(0),
		_size(0),
		_type(PLAIN)
{
	return ;
}

/**
 * @brief Copy constructor.
 * @param other The Resource object to copy.
 */
Resource::Resource(const Resource &other)
{
	*this = other;
	return ;
}

/**
 * @brief Assignment operator.
 * @param other The Resource object to assign from.
 * @return Reference to this Resource object.
 */
Resource	&Resource::operator=(const Resource &other)
{
	if (this != &other)
	{
		_path = other._path;
		_status = other._status;
		_size = other._size;
		_type = other._type;
	}
	return (*this);
}

/**
 * @brief Destructor.
 */
Resource::~Resource(void)
{
	return ;
}

/**
 * @brief Builds the resource object based on request target and server config.
 * 
 * @param requestTarget Requested path from the HTTP request.
 * @param config Server configuration.
 */
void	Resource::build(const std::string &requestTarget, const Config &config)
{
	if (_resolvePath(requestTarget, config) == SERV_ERROR)
		return ;
	_evaluatePermissions();
	_detectType();
	return ;
}

/**
 * @brief Resolves the filesystem path for the resource.
 *
 * Combines the server home directory with the requested target. If the path
 * is a directory, appends the server default page (e.g., index.html). Marks
 * the resource as existing even if permissions prevent accessing it (EACCES).
 * 
 * @note Marks as existing even if stat fails due to permission.
 * 
 * @param requestTarget Requested path from the HTTP request.
 * @param config Server configuration.
 * 
 * @return 0 if the path can be resolved, `SERV_ERROR` otherwise.
 */
int	Resource::_resolvePath(const std::string &requestTarget, const Config &config)
{
	struct stat	fileStat;
	struct stat	indexStat;

	_path = config.server_home + requestTarget;
	_status &= ~(EXISTS | IS_DIR);
	if (stat(_path.c_str(), &fileStat) == -1)
	{
		if (errno == EACCES)
			_status |= EXISTS;
		return (SERV_ERROR);
	}
	_status |= EXISTS;
	if (S_ISDIR(fileStat.st_mode))
	{
		std::string	indexPath = _path + config.default_page;
		if (stat(indexPath.c_str(), &indexStat) == 0)
			_path = indexPath;
		else
		{
			_status |= IS_DIR;
			return (SERV_ERROR);
		}
	}
	return (0);
}

/**
 * @brief Evaluates filesystem permissions for the resource.
 *
 * @note Does nothing if the resource path cannot be accessed.
 */
void	Resource::_evaluatePermissions(void)
{
	struct stat	fileStat;

	_status &= ~(PERM_ROK | PERM_WOK | PERM_XOK);
	if (stat(_path.c_str(), &fileStat) == -1)
		return ;
	if (fileStat.st_mode & S_IRUSR)
		_status |= PERM_ROK;
	if (fileStat.st_mode & S_IWUSR)
		_status |= PERM_WOK;
	if (fileStat.st_mode & S_IXUSR)
		_status |= PERM_XOK;
}

/**
 * @brief Detects the type of the resource based on the file extension.
 * 
 * Updates _status to include IS_CGI for Python and PHP scripts.
 */
void	Resource::_detectType(void)
{
	_status &= ~IS_CGI;
	if (utils::endsWith(_path, ".py"))
	{
		_status |= IS_CGI;
		_type = CGI_PY;
	}
	else if (utils::endsWith(_path, ".php"))
	{
		_status |= IS_CGI;
		_type = CGI_PHP;
	}
	else if (utils::endsWith(_path, ".html"))
		_type = HTML;
	else if (utils::endsWith(_path, ".jpeg"))
		_type = JPEG;
	else if (utils::endsWith(_path, ".png"))
		_type = PNG;
	else
		_type = PLAIN;
	return ;
}

/**
 * @brief Get MIME type string based on detected content type.
 * 
 * @return The MIME type (e.g., "text/html", "image/png").
 */
std::string	Resource::getMimeType(void) const
{
	switch(_type)
	{
		case HTML:	return "text/html";
		case PLAIN:	return "text/plain";
		case JPEG:	return "image/jpeg";
		case PNG:	return "image/png";
		default:	return "";
	}
}

/**
 * @brief Get the file extension associated with the content type.
 * 
 * @return File extension (e.g., ".html", ".png", ".php").
 */
std::string	Resource::getExtension(void) const
{
	switch(_type)
	{
		case HTML:		return ".html";
		case JPEG:		return ".jpeg";
		case PNG:		return ".png";
		case CGI_PY:	return ".py";
		case CGI_PHP:	return ".php";
		default:		return "";
	}
}

/**
 * @brief Returns the path.
 * @return Reference to the path.
 */
const std::string	&Resource::getPath(void) const
{
	return (_path);
}

/**
 * @brief Returns the status.
 * @return Status.
 */
int	Resource::getStatus(void) const
{
	return (_status);
}

/**
 * @brief Returns the size.
 * @return Size.
 */
size_t	Resource::getSize(void) const
{
	return (_size);
}

/**
 * @brief Returns the type.
 * @return Type.
 */
ContentTypes	Resource::getType(void) const
{
	return (_type);
}

/**
 * @brief Checks if the resource exists.
 * @return True if it is, false otherwise.
 */
bool	Resource::exists(void) const
{
	return (_status & EXISTS);
}

/**
 * @brief Checks if the resource is a CGI.
 * @return True if it is, false otherwise.
 */
bool	Resource::isCGI(void) const
{
	return (_status & IS_CGI);
}

/**
 * @brief Checks if the resource is a directory.
 * @return True if it is, false otherwise.
 */
bool	Resource::isDirectory(void) const
{
	return (_status & IS_DIR);
}

/**
 * @brief Checks if the resource is readable.
 * @return True if it is, false otherwise.
 */
bool	Resource::isReadable(void) const
{
	return (_status & PERM_ROK);
}

/**
 * @brief Checks if the resource is writable.
 * @return True if it is, false otherwise.
 */
bool	Resource::isWritable(void) const
{
	return (_status & PERM_WOK);
}

/**
 * @brief Checks if the resource is executable.
 * @return True if it is, false otherwise.
 */
bool	Resource::isExecutable(void) const
{
	return (_status & PERM_XOK);
}

/**
 * @brief Checks if an existing resource is forbidden.
 * @return True if it is, false otherwise.
 */
bool	Resource::isForbidden(void) const
{
	return (_status & EXISTS && !(_status & (PERM_ROK | PERM_WOK | PERM_XOK)));
}
