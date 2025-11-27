/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLexer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:15 by ego               #+#    #+#             */
/*   Updated: 2025/11/26 13:53:05 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"

enum	TokenType
{
	TOKEN_WORD,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_SEMICOLON,
	TOKEN_EOF
};

struct	Token
{
	TokenType	type;
	std::string	value;
	int			line;
};

/**
 * @brief Lexes raw config text into a token stream.
 */
class	ConfigLexer
{
	public:
		ConfigLexer(const std::string &input);

		std::vector<Token>	tokenize();

	private:
		const std::string	&_input;
		size_t				_pos;
		int					_line;

		bool	_eof() const;
		char	_peek() const;
		char	_get();
		void	_skipJunk();
		Token	_newToken(TokenType type, const std::string &value, int line) const;
		Token	_readWord();
		Token	_getNextToken();
};
