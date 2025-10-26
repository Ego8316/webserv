/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:57:27 by ego               #+#    #+#             */
/*   Updated: 2025/10/23 15:14:14 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "Config.hpp"
#include "Request.hpp"

class	Config;
class	Request;

/**
 * @class Resource
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

		void				build(const Request &request, const Config &config);

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
		
		bool	_checkRedirect(const std::string &requestTarget, const Config &config);
		bool	_checkAccept(const Request &request);
		int		_resolvePath(const std::string &requestTarget, const Config &config);
		void	_evaluatePermissions();
		void	_detectType();
};
