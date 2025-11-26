/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:08:49 by ego               #+#    #+#             */
/*   Updated: 2025/11/26 14:11:53 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.hpp"

std::string	tokenTypeToStr(TokenType t)
{
	switch (t)
	{
		case TOKEN_WORD:		return "WORD";
		case TOKEN_LBRACE:		return "LBRACE";
		case TOKEN_RBRACE:		return "RBRACE";
		case TOKEN_SEMICOLON:	return "SEMICOLON";
		case TOKEN_EOF:			return "EOF";
	}
	return "UNKNOWN";
}

void	printTokens(const std::vector<Token> &tokens)
{
	for (size_t i = 0; i < tokens.size(); ++i)
	{
		const Token	&t = tokens[i];
		std::cout << "[" << tokenTypeToStr(t.type) << "]"
				<< " line " << t.line
				<< " value='" << t.value << "'" << std::endl;
		if (t.type == TOKEN_EOF)
			break ;
	}
}
