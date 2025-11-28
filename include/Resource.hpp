/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:57:27 by ego               #+#    #+#             */
/*   Updated: 2025/11/24 23:40:58 by ego              ###   ########.fr       */
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
		Resource(const Resource &other);
		Resource	&operator=(const Resource &other);
		~Resource();

		void				build(const Request &request, const ServerConfig &config);

		const std::string	&getPath() const;
		ResourceStatus		getStatus() const;
		size_t				getSize() const;
		ContentType		getType() const;

		bool	exists() const;
		bool	isCGI() const;
		bool	isRedirect() const;
		bool	isDirectory() const;
		bool	isReadable() const;
		bool	isWritable() const;
		bool	isExecutable() const;
		bool	isForbidden() const;

	private:
		std::string		_path;
		ResourceStatus	_status;
		size_t			_size;
		ContentType	_type;
		HttpStatus		_redir_code;
		
		bool	_checkRedirect(const std::string &requestTarget, const ServerConfig &config);
		bool	_checkAccept(const Request &request);
		int		_resolvePath(const std::string &requestTarget, const ServerConfig &config);
		void	_evaluatePermissions();
		void	_detectType();
};
