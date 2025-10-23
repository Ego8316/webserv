/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 22:18:46 by ego               #+#    #+#             */
/*   Updated: 2025/10/23 15:14:14 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Resource.hpp"

/**
 * @brief Default constructor. Initializes status code to 404 NOT FOUND and
 * and type to PLAIN.
 */
Resource::Resource()
	:	_path(""),
		_status(EXISTS),
		_size(0),
		_type(FTYPE_ANY)
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
Resource::~Resource()
{
	return ;
}

/**
 * @brief Builds the resource object based on request target and server config.
 * 
 * @param requestTarget Requested path from the HTTP request.
 * @param config Server configuration.
 */
void	Resource::build(const Request &request, const Config &config)
{
	if (_checkRedirect(request.getRequestTarget(), config))
		return ;
	if (_resolvePath(request.getRequestTarget(), config) == SERV_ERROR)
		return ;
	_evaluatePermissions();
	_detectType();
	_checkAccept(request);
	return ;
}

bool	Resource::_checkRedirect(const std::string &requestTarget, const Config &config)
{
	std::string							raw_path_requested(requestTarget);
	std::map<std::string, Redirection>	redirs = config.getRedirections();
	
	for (std::map<std::string, Redirection>::iterator it = redirs.begin(); it != redirs.end(); ++it)
	{
		if ((raw_path_requested == it->first) || (raw_path_requested + config.default_page == it->first))
		{
			_path = it->second.dest;
			_redir_code = it->second.error_code;
			_status = IS_REDIRECT;
			return (true);
		}	
	}
	return (false);
}

bool	Resource::_checkAccept(const Request &request)
{

	if (!(this->_type & request.getAccept()))
		_status = static_cast<ResourceStatus>(_status | ACCEPT_ERROR);
	return (this->_type & request.getAccept());
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
	struct stat	file_stat;

	_path = config.server_home + requestTarget;
	_status = static_cast<ResourceStatus>(_status & ~(EXISTS | IS_DIR));
	if (stat(_path.c_str(), &file_stat) == -1)
	{
		if (errno == EACCES)
			_status = static_cast<ResourceStatus>(_status | EXISTS);
		std::cerr << "Cannot find ressource " << _path << std::endl;
		return (SERV_ERROR);
	}
	_status = static_cast<ResourceStatus>(_status | EXISTS);
	if (S_ISDIR(file_stat.st_mode))
	{
		std::string	index_path = _path + config.default_page;
		if (stat(index_path.c_str(), &file_stat) == 0)
			_path = index_path;
		else
			_status = static_cast<ResourceStatus>(_status | IS_DIR);
	}
	return (0);
}

/**
 * @brief Evaluates filesystem permissions for the resource.
 *
 * @note Does nothing if the resource path cannot be accessed.
 */
void	Resource::_evaluatePermissions()
{
	struct stat	file_stat;

	_status = static_cast<ResourceStatus>(_status & ~(PERM_ROK | PERM_WOK | PERM_XOK));
	if (stat(_path.c_str(), &file_stat) == -1)
		return ;
	if (file_stat.st_mode & S_IRUSR)
		_status = static_cast<ResourceStatus>(_status | PERM_ROK);
	if (file_stat.st_mode & S_IWUSR)
		_status = static_cast<ResourceStatus>(_status | PERM_WOK);
	if (file_stat.st_mode & S_IXUSR)
		_status = static_cast<ResourceStatus>(_status | PERM_XOK);
}

/**
 * @brief Detects the type of the resource based on the file extension.
 * 
 * Updates _status to include IS_CGI for Python and PHP scripts.
 */
void	Resource::_detectType()
{
	_type = utils::extensionToContentTypes(_path);
	_status = static_cast<ResourceStatus>(_status & ~IS_CGI);
	_status = static_cast<ResourceStatus>(_status | (IS_CGI * ((FTYPE_IS_CGI & _type) != 0)));
	return ;
}

/**
 * @brief Returns the path.
 * @return Reference to the path.
 */
const std::string	&Resource::getPath() const
{
	return (_path);
}

/**
 * @brief Returns the status.
 * @return Status.
 */
ResourceStatus	Resource::getStatus() const
{
	return (_status);
}

/**
 * @brief Returns the size.
 * @return Size.
 */
size_t	Resource::getSize() const
{
	return (_size);
}

/**
 * @brief Returns the type.
 * @return Type.
 */
ContentType	Resource::getType() const
{
	return (_type);
}

/**
 * @brief Checks if the resource exists.
 * @return True if it is, false otherwise.
 */
bool	Resource::exists() const
{
	return (_status & EXISTS);
}

/**
 * @brief Checks if the resource is a CGI.
 * @return True if it is, false otherwise.
 */
bool	Resource::isCGI() const
{
	return (_status & IS_CGI);
}

bool	Resource::isRedirect() const
{
	return (_status & IS_REDIRECT);
}

/**
 * @brief Checks if the resource is a directory.
 * @return True if it is, false otherwise.
 */
bool	Resource::isDirectory() const
{
	return (_status & IS_DIR);
}

/**
 * @brief Checks if the resource is readable.
 * @return True if it is, false otherwise.
 */
bool	Resource::isReadable() const
{
	return (_status & PERM_ROK);
}

/**
 * @brief Checks if the resource is writable.
 * @return True if it is, false otherwise.
 */
bool	Resource::isWritable() const
{
	return (_status & PERM_WOK);
}

/**
 * @brief Checks if the resource is executable.
 * @return True if it is, false otherwise.
 */
bool	Resource::isExecutable() const
{
	return (_status & PERM_XOK);
}

/**
 * @brief Checks if an existing resource is forbidden.
 * @return True if it is, false otherwise.
 */
bool	Resource::isForbidden() const
{
	return (_status & EXISTS && !(_status & (PERM_ROK | PERM_WOK | PERM_XOK)));
}
