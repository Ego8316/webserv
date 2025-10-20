/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:57:27 by ego               #+#    #+#             */
/*   Updated: 2025/10/20 19:44:08 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "Config.hpp"
#include "Request.hpp"

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
		Resource(void);
		Resource(const Resource &other);
		Resource	&operator=(const Resource &other);
		~Resource(void);

		void				build(const Request &request, const Config &config);

		const std::string	&getPath(void) const;
		ResourceStatus		getStatus(void) const;
		size_t				getSize(void) const;
		HttpStatus			getHttpStatus(void)	const;
		ContentTypes		getType(void) const;

		bool	exists(void) const;
		bool	isCGI(void) const;
		bool	isRedirect(void) const;
		bool	isDirectory(void) const;
		bool	isReadable(void) const;
		bool	isWritable(void) const;
		bool	isExecutable(void) const;
		bool	isForbidden(void) const;

	private:
		std::string		_path;
		ResourceStatus	_status;
		size_t			_size;
		ContentTypes	_type;
		HttpStatus		_redir_code;
		
		bool	_checkRedirect(const std::string &requestTarget, const Config &config);
		bool	_checkAccept(const Request &request);
		int		_resolvePath(const std::string &requestTarget, const Config &config);
		void	_evaluatePermissions(void);
		void	_detectType(void);
};
