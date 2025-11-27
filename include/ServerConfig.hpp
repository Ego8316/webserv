/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 20:15:57 by ego               #+#    #+#             */
/*   Updated: 2025/11/27 16:26:26 by ego              ###   ########.fr       */
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

		std::string						server_name;
		std::string						listen_host_string;
		uint32_t						listen_host;
		int								listen_port;
		std::string						root;
		std::string						index;
		bool							autoindex;
		int								max_clients;
		size_t							client_max_body_size;
		long							client_header_timeout;
		long							client_body_timeout;
		long							send_timeout;
		size_t							client_header_buffer_size;
		size_t							client_body_buffer_size;
		std::map<std::string, Location>	locations;
		std::map<int, std::string>		error_pages;
};

std::ostream &operator<<(std::ostream &os, const ServerConfig &cfg);
