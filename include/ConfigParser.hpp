/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:22 by ego               #+#    #+#             */
/*   Updated: 2025/11/26 16:14:05 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"
#include "ConfigLexer.hpp"

struct	ASTDirective
{
	std::string					name;
	std::vector<std::string>	args;
	int							line;
};

struct	ASTBlock
{
	std::string					type;
	std::string					path;
	std::vector<ASTDirective>	directives;
	std::vector<ASTBlock>		children;
	int							line;
};

class	ConfigParser
{
	public:
		ConfigParser(const std::vector<Token> &tokens);

		std::vector<ASTBlock>	parse();

	private:
		const std::vector<Token>	&_tokens;
		size_t						_pos;

		const Token		&_peek() const;
		const Token		&_eat();
		bool			_eof() const;
		void			_expect(TokenType type, const std::string &msg);
		ASTBlock		_parseServerBlock();
		ASTBlock		_parseLocationBlock();
		ASTDirective	_parseDirective();
};

