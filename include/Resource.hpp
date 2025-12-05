/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vviterbo <vviterbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:57:27 by ego               #+#    #+#             */
/*   Updated: 2025/12/05 10:55:42 by vviterbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"

class	ServerConfig;
class	Request;

/**
 * @class Resource
 *
 * @brief Represents a filesystem resource for HTTP requests.
 *
 * Handles path resolution, permission evaluation, and type detection.
 */
class	Resource
{
	public:
		Resource();
		~Resource();

		void				build(const Request &request, const ServerConfig &config);

		const std::string	&getPath() const;
		ResourceStatus		getStatus() const;
		size_t				getSize() const;
		ContentType			getType() const;
		bool				methodAllowed() const;
		bool				autoindex() const;
		HttpStatus			getRedirectCode() const;
		const Location		*getLocation() const;

		bool	exists() const;
		bool	isCGI() const;
		bool	isRedirect() const;
		bool	isDirectory() const;
		bool	isReadable() const;
		bool	isWritable() const;
		bool	isExecutable() const;
		bool	isForbidden() const;
		bool	isHidden() const;

	private:
		std::string		_path;
		ResourceStatus	_status;
		size_t			_size;
		ContentType		_type;
		HttpStatus		_redir_code;
		bool			_method_allowed;
		bool			_autoindex;
		std::string		_index;
		bool			_was_dir;
		const Location	*_loc;
		
		bool	_checkHidden(const std::string &requestTarget);
		bool	_checkRedirect(const Location *loc);
		bool	_checkAccept(const Request &request);
		int		_resolvePath(const std::string &requestTarget, const std::string &root, const std::string &index);
		void	_evaluatePermissions();
		void	_detectType();
};
