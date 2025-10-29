/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 22:18:46 by ego               #+#    #+#             */
/*   Updated: 2025/10/29 16:35:01 by victorviter      ###   ########.fr       */
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
	this->_evaluatePermissions();
	this->_detectType();
	this->_checkAccept(request);
	return ;
}

bool	Resource::_checkRedirect(const std::string &requestTarget, const Config &config)
{
	std::string							raw_path_requested(requestTarget);
	// TODO
	std::map<std::string, Redirection>	redirs;
	
	(void)config;
	for (std::map<std::string, Redirection>::iterator it = redirs.begin(); it != redirs.end(); ++it)
	{
		// TODO adapter avec location
		// if ((raw_path_requested == it->first) || (raw_path_requested + config.default_page == it->first))
		if ((raw_path_requested == it->first) || (raw_path_requested == it->first))
		{
			this->_path = it->second.dest;
			this->_redir_code = it->second.error_code;
			this->_status = IS_REDIRECT;
			return (true);
		}	
	}
	return (false);
}

bool	Resource::_checkAccept(const Request &request)
{

	if (!(this->_type & request.getAccept()))
		this->_status |= ACCEPT_ERROR;
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

	this->_path = config.server_home + requestTarget;
	this->_status = static_cast<ResourceStatus>(this->_status & ~(EXISTS | IS_DIR));
	if (stat(this->_path.c_str(), &file_stat) == -1)
	{
		if (errno == EACCES)
			this->_status |= EXISTS;
		std::cerr << "Cannot find ressource " << this->_path << std::endl;
		return (SERV_ERROR);
	}
	this->_status |= EXISTS;
	if (S_ISDIR(file_stat.st_mode))
	{
		// TODO adapter avec Location
		// std::string	index_path = _path + config.default_page;
		std::string	index_path = this->_path;
		if (stat(index_path.c_str(), &file_stat) == 0)
			this->_path = index_path;
		else
			this->_status |= IS_DIR;
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

	this->_status = static_cast<ResourceStatus>(this->_status & ~(PERM_ROK | PERM_WOK | PERM_XOK));
	if (stat(_path.c_str(), &file_stat) == -1)
		return ;
	if (file_stat.st_mode & S_IRUSR)
		this->_status |= PERM_ROK;
	if (file_stat.st_mode & S_IWUSR)
		this->_status |= PERM_WOK;
	if (file_stat.st_mode & S_IXUSR)
		this->_status |= PERM_XOK;
}

/**
 * @brief Detects the type of the resource based on the file extension.
 * 
 * Updates _status to include IS_CGI for Python and PHP scripts.
 */
void	Resource::_detectType()
{
	this->_type = utils::extensionToContentTypes(_path);
	this->_status = static_cast<ResourceStatus>(_status & ~IS_CGI);
	this->_status = static_cast<ResourceStatus>(_status | (IS_CGI * ((FTYPE_IS_CGI & _type) != 0)));
	return ;
}

/**
 * @brief Returns the path.
 * @return Reference to the path.
 */
const std::string	&Resource::getPath() const
{
	return (this->_path);
}

/**
 * @brief Returns the status.
 * @return Status.
 */
ResourceStatus	Resource::getStatus() const
{
	return (this->_status);
}

/**
 * @brief Returns the size.
 * @return Size.
 */
size_t	Resource::getSize() const
{
	return (this->_size);
}

/**
 * @brief Returns the type.
 * @return Type.
 */
ContentType	Resource::getType() const
{
	return (this->_type);
}

/**
 * @brief Checks if the resource exists.
 * @return True if it is, false otherwise.
 */
bool	Resource::exists() const
{
	return (this->_status & EXISTS);
}

/**
 * @brief Checks if the resource is a CGI.
 * @return True if it is, false otherwise.
 */
bool	Resource::isCGI() const
{
	return (this->_status & IS_CGI);
}

bool	Resource::isRedirect() const
{
	return (this->_status & IS_REDIRECT);
}

/**
 * @brief Checks if the resource is a directory.
 * @return True if it is, false otherwise.
 */
bool	Resource::isDirectory() const
{
	return (this->_status & IS_DIR);
}

/**
 * @brief Checks if the resource is readable.
 * @return True if it is, false otherwise.
 */
bool	Resource::isReadable() const
{
	return (this->_status & PERM_ROK);
}

/**
 * @brief Checks if the resource is writable.
 * @return True if it is, false otherwise.
 */
bool	Resource::isWritable() const
{
	return (this->_status & PERM_WOK);
}

/**
 * @brief Checks if the resource is executable.
 * @return True if it is, false otherwise.
 */
bool	Resource::isExecutable() const
{
	return (this->_status & PERM_XOK);
}

/**
 * @brief Checks if an existing resource is forbidden.
 * @return True if it is, false otherwise.
 */
bool	Resource::isForbidden() const
{
	return (this->_status & EXISTS && !(this->_status & (PERM_ROK | PERM_WOK | PERM_XOK)));
}
