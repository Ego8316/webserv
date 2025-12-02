/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 20:15:57 by ego               #+#    #+#             */
/*   Updated: 2025/12/02 17:04:06 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

struct	Redirection
{
	int			code;
	std::string	url;
};

struct	Location
{
	std::string							path;
	std::string							root;
	std::string							index;
	bool								autoindex;
	Method								methods;
	std::string							upload_path;
	std::string							cgi_pass;
	Redirection							redirect;
	bool								has_redirect;
};

/**
 * @brief Runtime representation of a single server block.
 */
class	ServerConfig
{
	public:
		ServerConfig();

		const Location	*matchLocation(const std::string &path) const;

		static const unsigned int		client_limit = 1000;
		static const unsigned int		incoming_queue_backlog = 1000;
		std::string						server_name;
		std::string						listen_host_string;
		uint32_t						listen_host;
		int								listen_port;
		std::string						root;
		std::string						index;
		bool							autoindex;
		int								max_clients;
		size_t							client_max_body_size;
		long							timeout;
		size_t							client_header_buffer_size;
		size_t							client_body_buffer_size;
		std::map<std::string, Location>	locations;
		std::map<int, std::string>		error_pages;
};

std::ostream &operator<<(std::ostream &os, const ServerConfig &cfg);
