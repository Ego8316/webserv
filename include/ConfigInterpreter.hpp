/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigInterpreter.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:06 by ego               #+#    #+#             */
/*   Updated: 2025/11/27 03:40:15 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <set>
#include "ConfigParser.hpp"
#include "ServerConfig.hpp"
#include "ConfigError.hpp"

/**
 * @brief Interprets parsed blocks into runnable server configurations.
 */
class	ConfigInterpreter
{
	public:
		ConfigInterpreter(const std::vector<Block> &blocks);

		std::vector<ServerConfig>	interpret();
	private:
		const std::vector<Block>	&_blocks;

		ServerConfig	_parseServer(const Block &block);
		Location		_parseLocation(const Block &block, const ServerConfig &server_defaults);
	
		void			_parseErrorPages(ServerConfig &conf, const Block &block,
						bool &seen_error_pages_block);
		void			_applyServerDirective(ServerConfig  &conf,  const Directive &d,
						std::set<std::string> &already_applied);
		void			_applyLocationDirective(Location &loc, const Directive &d,
						std::set<std::string> &already_applied);

		void			_parseListen(ServerConfig &conf, const Directive &d);
		void			_parseErrorPage(ServerConfig &conf, const Directive  &d);
		void			_parseMethod(Location &loc, const Directive &d);
		void			_parseReturn(Location &loc,  const Directive &d);

		size_t			_parseSizeWithSuffix(const std::string &s, int line) const;
};
