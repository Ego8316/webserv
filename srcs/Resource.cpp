/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 22:18:46 by ego               #+#    #+#             */
/*   Updated: 2025/11/29 18:41:52 by ego              ###   ########.fr       */
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
		_type(FTYPE_ANY),
		_redir_code(HTTP_UNKNOWN_STATUS),
		_method_allowed(true),
		_autoindex(false),
		_index("")
{
	return ;
}

/**
 * @brief Copy constructor.
 *
 * @param other The Resource object to copy.
 */
Resource::Resource(const Resource &other)
{
	*this = other;
	return ;
}

/**
 * @brief Assignment operator.
 *
 * @param other The Resource object to assign from.
 *
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
		_redir_code = other._redir_code;
		_method_allowed = other._method_allowed;
		_autoindex = other._autoindex;
		_index = other._index;
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
void	Resource::build(const Request &request, const ServerConfig &config)
{
	this->_status = static_cast<ResourceStatus>(0);
	this->_type = FTYPE_ANY;
	this->_redir_code = HTTP_UNKNOWN_STATUS;
	this->_method_allowed = (request.getMethod() & GET);
	this->_autoindex = config.autoindex;
	this->_index = config.index;
	this->_path.clear();
	this->_size = 0;

	const Location	*loc = config.matchLocation(request.getRequestTarget());
	std::string		root = config.root;
	std::string		target = request.getRequestTarget();

	if (loc)
	{
		_method_allowed = (loc->methods & request.getMethod());
		_autoindex = loc->autoindex;
		_index = loc->index;
		if (_checkRedirect(loc))
			return ;
		if (!loc->root.empty())
			root = loc->root;
		if (target.compare(0, loc->path.size(), loc->path) == 0)
			target = target.substr(loc->path.size());
		if (!loc->upload_path.empty() && request.getMethod() == POST)
			root = loc->upload_path;
		if (!loc->cgi_pass.empty())
			this->_status = static_cast<ResourceStatus>(this->_status | IS_CGI);
	}
	if (target.empty() || target[0] != '/')
		target.insert(0, "/");
	if (_resolvePath(target, root, _index) == SERV_ERROR)
		return ;
	this->_evaluatePermissions();
	this->_detectType();
	this->_checkAccept(request);
	return ;
}

/**
 * @brief Detects configured redirects matching the requested target.
 *
 * @param requestTarget Requested path.
 * @param config Server configuration.
 *
 * @return True when a redirect is found.
 */
bool	Resource::_checkRedirect(const Location *loc)
{
	if (!loc || !loc->has_redirect)
		return (false);
	this->_status = IS_REDIRECT;
	this->_path = loc->redirect.url;
	this->_redir_code = static_cast<HttpStatus>(loc->redirect.code);
	return (true);
}

/**
 * @brief Validates the request Accept header against detected resource type.
 *
 * @param request Client request (provides Accept header).
 *
 * @return True when accepted, false otherwise.
 */
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
int	Resource::_resolvePath(const std::string &requestTarget, const std::string &root, const std::string &index)
{
	struct stat	file_stat;

	this->_path = root + requestTarget;
	this->_status = static_cast<ResourceStatus>(this->_status & ~(EXISTS | IS_DIR));
	if (stat(this->_path.c_str(), &file_stat) == -1)
	{
		if (errno == EACCES)
			this->_status |= EXISTS;
		std::cerr << "Cannot find ressource " << this->_path << std::endl;
		return (SERV_ERROR);
	}
	this->_status |= EXISTS;
	if (!S_ISDIR(file_stat.st_mode))
		this->_size = static_cast<size_t>(file_stat.st_size);
	if (S_ISDIR(file_stat.st_mode))
	{
		std::string base = this->_path;
		if (!utils::endsWith(base, "/"))
			base += "/";
		std::string	index_path = base + index;
		if (stat(index_path.c_str(), &file_stat) == 0)
		{
			this->_path = index_path;
			this->_size = static_cast<size_t>(file_stat.st_size);
		}
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
	bool	was_cgi = (_status & IS_CGI);

	this->_type = utils::extensionToContentTypes(_path);
	this->_status = static_cast<ResourceStatus>(_status & ~IS_CGI);
	this->_status = static_cast<ResourceStatus>(_status | (IS_CGI * ((FTYPE_IS_CGI & _type) != 0)));
	if (was_cgi)
		this->_status = static_cast<ResourceStatus>(_status | IS_CGI);
	return ;
}

/**
 * @brief Returns the path.
 *
 * @return Reference to the path.
 */
const std::string	&Resource::getPath() const
{
	return (this->_path);
}

bool	Resource::methodAllowed() const
{
	return (this->_method_allowed);
}

bool	Resource::autoindex() const
{
	return (this->_autoindex);
}

HttpStatus	Resource::getRedirectCode() const
{
	return (this->_redir_code);
}

/**
 * @brief Returns the status.
 *
 * @return Status.
 */
ResourceStatus	Resource::getStatus() const
{
	return (this->_status);
}

/**
 * @brief Returns the size.
 *
 * @return Size.
 */
size_t	Resource::getSize() const
{
	return (this->_size);
}

/**
 * @brief Returns the type.
 *
 * @return Type.
 */
ContentType	Resource::getType() const
{
	return (this->_type);
}

/**
 * @brief Checks if the resource exists.
 *
 * @return True if it is, false otherwise.
 */
bool	Resource::exists() const
{
	return (this->_status & EXISTS);
}

/**
 * @brief Checks if the resource is a CGI.
 *
 * @return True if it is, false otherwise.
 */
bool	Resource::isCGI() const
{
	return (this->_status & IS_CGI);
}

/**
 * @brief Returns true when the resource resolves to a redirect.
 *
 * @return True if redirect.
 */
bool	Resource::isRedirect() const
{
	return (this->_status & IS_REDIRECT);
}

/**
 * @brief Checks if the resource is a directory.
 *
 * @return True if it is, false otherwise.
 */
bool	Resource::isDirectory() const
{
	return (this->_status & IS_DIR);
}

/**
 * @brief Checks if the resource is readable.
 *
 * @return True if it is, false otherwise.
 */
bool	Resource::isReadable() const
{
	return (this->_status & PERM_ROK);
}

/**
 * @brief Checks if the resource is writable.
 *
 * @return True if it is, false otherwise.
 */
bool	Resource::isWritable() const
{
	return (this->_status & PERM_WOK);
}

/**
 * @brief Checks if the resource is executable.
 *
 * @return True if it is, false otherwise.
 */
bool	Resource::isExecutable() const
{
	return (this->_status & PERM_XOK);
}

/**
 * @brief Checks if an existing resource is forbidden.
 *
 * @return True if it is, false otherwise.
 */
bool	Resource::isForbidden() const
{
	return (this->_status & EXISTS && !(this->_status & (PERM_ROK | PERM_WOK | PERM_XOK)));
}
