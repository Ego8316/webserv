/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 01:36:17 by ego               #+#    #+#             */
/*   Updated: 2025/11/28 13:13:42 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

/**
 * @brief Construct a server configuration with defaults.
 *
 * These values are overridden by parsed configuration files. Defaults mirror a
 * minimal HTTP server with conservative limits.
 */
ServerConfig::ServerConfig()
	:	listen_host_string("0.0.0.0"),
		listen_host(htonl(0x00000000)),
		listen_port(80),
		root(""),
		index("index.html"),
		autoindex(false),
		max_clients(1000),
		client_max_body_size(1 << 20),
		client_header_timeout(10),
		client_body_timeout(60),
		send_timeout(60),
		client_header_buffer_size(1024),
		client_body_buffer_size(8 * 1024)
{
	return ;	
}

/**
 * @brief Find the best-matching location block for a path.
 *
 * Selects the longest prefix match among configured locations. Returns NULL
 * when no location matches.
 *
 * @param path Normalized request target (starting with '/').
 *
 * @return Pointer to the matching Location or NULL.
 */
const Location	*ServerConfig::matchLocation(const std::string &path) const
{
	const Location	*best = NULL;
	size_t			best_len = 0;

	for (std::map<std::string, Location>::const_iterator it = locations.begin();
			it != locations.end(); ++it)
	{
		const std::string &prefix = it->first;
		if (path.compare(0, prefix.size(), prefix) == 0 && prefix.size() >= best_len)
		{
			best = &it->second;
			best_len = prefix.size();
		}
	}
	return (best);
}

/**
 * @brief Print the top border of the formatted configuration table.
 *
 * @param os Output stream.
 * @param title Title text to embed in the border.
 */
static void	printBorderTop(std::ostream &os, const std::string &title)
{
	os << BORDER_COLOR << TOP_LEFT << title;
	for (size_t i = 0; i < WIDTH - title.size(); ++i) { os << HORIZONTAL; }
	os << TOP_RIGHT << RESET << "\n";
}

/**
 * @brief Print the bottom border of the formatted table.
 *
 * @param os Output stream.
 */
static void	printBorderBottom(std::ostream &os)
{
	os << BORDER_COLOR << BOTTOM_LEFT;
	for (size_t i = 0; i < WIDTH; ++i) { os << HORIZONTAL; }
	os << BOTTOM_RIGHT << RESET << "\n";
}

/**
 * @brief Print a section header row with optional subtitle.
 *
 * @param os Output stream.
 * @param title Section title.
 * @param subtitle Right-aligned subtitle text.
 */
static void	printSection(std::ostream &os, const std::string &title, const std::string &subtitle)
{
	os << BORDER_COLOR << VERTICAL << RESET << " ";
	os << SECTION_COLOR << title << RESET;
	for (size_t i = 0; i <= WIDTH / 2 - (int)title.size(); ++i) { os << " "; }
	if (subtitle.size() < WIDTH / 2 - 3)
	{
		os << SECTION_SUB_COLOR << subtitle << RESET;
		for (size_t i = 0; i < WIDTH / 2 - 2 - subtitle.size(); ++i) { os << " "; }
	}
	else
		for (size_t i = 0; i < WIDTH / 2 - 2; ++i) { os << " "; }
	os << BORDER_COLOR << VERTICAL << RESET << "\n";
}

/**
 * @brief Print a name/value field on one row of the table.
 *
 * @param os Output stream.
 * @param name Field label.
 * @param value Field value.
 */
static void	printField(std::ostream &os, const std::string &name, const std::string &value)
{
	os << BORDER_COLOR << VERTICAL << RESET << "   ";
	if (name.length() < WIDTH / 2)
	{
		os << FIELD_NAME_COLOR << name << RESET;
		for (size_t i = 0; i < WIDTH / 2 - name.length(); ++i) { os << " "; }
	}
	else
		for (size_t i = 0; i < WIDTH / 2; ++i) { os << " "; }

	if (value.length() < WIDTH / 2 - 3)
	{
		os << FIELD_VALUE_COLOR << value << RESET;
		for (size_t i = 0; i < WIDTH / 2 - 3 - value.length(); ++i) { os << " "; }
	}
	else
		for (size_t i = 0; i < WIDTH / 2 - 3; ++i) { os << " "; }
	os << BORDER_COLOR << VERTICAL << RESET << "\n";
}

/**
 * @brief Render server-level settings rows.
 *
 * @param os Output stream.
 * @param cfg Configuration to read.
 */
static void	printServerSettings(std::ostream &os, const ServerConfig &cfg)
{
	printSection(os, "Server Settings", "");
	printField(os, "Server name:", cfg.server_name);
	printField(os, "Listen host:", cfg.listen_host_string);
	printField(os, "Listen port:", utils::toString(cfg.listen_port));
	printField(os, "Root:", cfg.root);
	printField(os, "Index:", cfg.index);
	printField(os, "Autoindex:", cfg.autoindex ? "ON" : "OFF");
	printField(os, "Max body size:", utils::toString(cfg.client_max_body_size));
}

/**
 * @brief Render timeout-related fields.
 *
 * @param os Output stream.
 * @param cfg Configuration to read.
 */
static void	printTimeouts(std::ostream &os, const ServerConfig &cfg)
{
	printSection(os, "Timeouts", "");
	printField(os, "Header timeout:", utils::toString(cfg.client_header_timeout) + "s");
	printField(os, "Body timeout:", utils::toString(cfg.client_body_timeout) + "s");
	printField(os, "Send timeout:", utils::toString(cfg.send_timeout) + "s");
}

/**
 * @brief Render buffer-related fields.
 *
 * @param os Output stream.
 * @param cfg Configuration to read.
 */
static void	printBuffers(std::ostream &os, const ServerConfig &cfg)
{
	printSection(os, "Buffers", "");
	printField(os, "Header buffer:", utils::toString(cfg.client_header_buffer_size));
	printField(os, "Body buffer:", utils::toString(cfg.client_body_buffer_size));
}

/**
 * @brief Render configured error pages.
 *
 * @param os Output stream.
 * @param cfg Configuration to read.
 */
static void	printErrorPages(std::ostream &os, const ServerConfig &cfg)
{
	printSection(os, "Error pages", "");
	for (std::map<int, std::string>::const_iterator it = cfg.error_pages.begin();
			it != cfg.error_pages.end(); ++it)
		printField(os, utils::toString(it->first), it->second);
}

/**
 * @brief Render a location block summary.
 *
 * @param os Output stream.
 * @param loc Location to render.
 */
static void	printLocation(std::ostream &os, const Location &loc)
{
	printSection(os, "Location", loc.path);
	printField(os, "Root:", loc.root);
	printField(os, "Index:", loc.index);
	printField(os, "Autoindex:", loc.autoindex ? "ON" : "OFF");
	std::string	mlist;
	if (loc.methods & GET)    mlist += "GET ";
	if (loc.methods & POST)   mlist += "POST ";
	if (loc.methods & DELETE) mlist += "DELETE ";
	printField(os, "Methods:", mlist);
	if (!loc.upload_path.empty())
		printField(os, "Upload store:", loc.upload_path);
	if (!loc.cgi_pass.empty())
		printField(os, "CGI pass:", loc.cgi_pass);
	if (loc.has_redirect)
		printField(os, "Redirect:",
					utils::toString(loc.redirect.code) + " -> " + loc.redirect.url);
}

/**
 * @brief Pretty-print a server configuration.
 *
 * Produces a table view used at startup to visualize active settings.
 *
 * @param os Output stream.
 * @param cfg Configuration to render.
 *
 * @return Reference to the output stream.
 */
std::ostream &operator<<(std::ostream &os, const ServerConfig &cfg)
{
	printBorderTop(os, "SERVER CONFIG");

	printServerSettings(os, cfg);
	printTimeouts(os, cfg);
	printBuffers(os, cfg);
	printErrorPages(os, cfg);
	for (std::map<std::string, Location>::const_iterator it = cfg.locations.begin();
	     it != cfg.locations.end(); ++it)
		printLocation(os, it->second);
	printBorderBottom(os);
	return os;
}
