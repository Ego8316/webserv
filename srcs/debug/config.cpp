/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:08:49 by ego               #+#    #+#             */
/*   Updated: 2025/11/27 02:39:54 by ego              ###   ########.fr       */
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

static void	printIndent(int depth)
{
	for (int i = 0; i < depth; ++i)
		std::cout << "   ";
	return ;
}

static void	printDirective(const Directive &d, int depth)
{
	printIndent(depth);
	std::cout << "directive " << d.name << ":";
	if (d.args.empty())
		std::cout << "<no args>";
	else
		for (size_t i = 0; i < d.args.size(); ++i)
			std::cout << " " << d.args[i];
	std::cout << std::endl;
	return ;
}

static void	printBlock(const Block &b, int depth)
{
	printIndent(depth);
	std::cout << b.type;
	if (!b.path.empty())
		std::cout << " " << b.path;
	std::cout << std::endl;
	for (size_t i = 0; i < b.directives.size(); ++i)
		printDirective(b.directives[i], depth + 1);
	for (size_t i = 0; i < b.children.size(); ++i)
		printBlock(b.children[i], depth + 1);
	printIndent(depth);
	std::cout << "end " << b.type << std::endl;
}

void	printBlocks(const std::vector<Block> &blocks)
{
	for (size_t i = 0; i < blocks.size(); ++i)
	{
		printBlock(blocks[i], 0);
		std::cout << "\n";
	}
	return ;
}

std::ostream	&operator<<(std::ostream &os, TokenType t)
{
	os << tokenTypeToStr(t);
	return (os);
}

std::ostream	&operator<<(std::ostream &os, Token t)
{
	os << "[" << t.type << "]"
		<< " line " << t.line
		<< " value='" << t.value << "'";
	return (os);
}

std::ostream	&operator<<(std::ostream &os, const std::vector<Token> &tokens)
{
	for (size_t i = 0; i < tokens.size(); ++i)
		os << tokens[i] << std::endl;
	return (os);
}
