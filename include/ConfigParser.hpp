/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:22 by ego               #+#    #+#             */
/*   Updated: 2025/11/27 15:55:40 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "ConfigLexer.hpp"
#include "ConfigError.hpp"

struct	Directive
{
	std::string					name;
	std::vector<std::string>	args;
	int							line;
};

struct	Block
{
	std::string				type;
	std::string				path;
	std::vector<Directive>	directives;
	std::vector<Block>		children;
	int						line;
};

/**
 * @brief Parses token streams into structured configuration blocks.
 */
class	ConfigParser
{
	public:
		ConfigParser(const std::vector<Token> &tokens);

		std::vector<Block>	parse();

	private:
		const std::vector<Token>	&_tokens;
		size_t						_pos;

		const Token		&_peek() const;
		const Token		&_eat();
		bool			_eof() const;
		void			_expect(TokenType type, const std::string &msg);
		Block		_parseServerBlock();
		Block		_parseErrorPagesBlock();
		Block		_parseLocationBlock();
		Directive	_parseDirective();
};
