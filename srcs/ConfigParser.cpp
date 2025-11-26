/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:42 by ego               #+#    #+#             */
/*   Updated: 2025/11/26 16:56:15 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::vector<Token> &tokens)
	:	_tokens(tokens),
		_pos(0)
{
	return ;
}

std::vector<ASTBlock>	ConfigParser::parse()
{
	std::vector<ASTBlock>	servers;

	while (!_eof())
	{
		const Token	&token = _peek();
		if (token.type == TOKEN_WORD && token.value == "server")
			servers.push_back(_parseServerBlock());
		else if (token.type == TOKEN_EOF)
			break ;
		else
			throw std::runtime_error("Unexpected token '" + token.value
				+ "' at line " + utils::toString(token.line)
				+ ": only 'server' blocks allowed at root");
	}
	return (servers);
}

const Token	&ConfigParser::_peek() const
{
	return (_tokens[_pos]);
}

const Token	&ConfigParser::_eat()
{
	if (_eof())
		throw std::runtime_error("Unexpected end of file at line "
			+ _peek().line);
	return (_tokens[_pos++]);
}

bool	ConfigParser::_eof() const
{
	return	(_tokens[_pos].type == TOKEN_EOF);
}

void	ConfigParser::_expect(TokenType type, const std::string &expected)
{
	if (_peek().type != type)
		throw std::runtime_error("Unexpected token at line "
			+ utils::toString(_peek().line) + ": expected '" + expected + "'");
	_eat();
	return ;
}

ASTBlock	ConfigParser::_parseServerBlock()
{
	ASTBlock	block;

	block.type = "server";
	block.path = "";
	block.line = _peek().line;
	_expect(TOKEN_WORD, "server");
	_expect(TOKEN_LBRACE, "{");
	while (!_eof() && _peek().type != TOKEN_RBRACE)
	{
		const Token	&token = _peek();
		if (token.type == TOKEN_WORD && token.value == "location")
			block.children.push_back(_parseLocationBlock());
		else if (token.type == TOKEN_WORD)
			block.directives.push_back(_parseDirective());
		else
			throw std::runtime_error("Unexpected token inside server block on line "
				+ utils::toString(token.line));
	}
	_expect(TOKEN_RBRACE, "}");
	return (block);
}

ASTBlock	ConfigParser::_parseLocationBlock()
{
	ASTBlock	block;

	block.type = "location";
	block.line = _peek().line;
	_expect(TOKEN_WORD, "location");
	if (_peek().type != TOKEN_WORD)
		throw std::runtime_error("Expected path after 'location' at line "
			+ utils::toString(_peek().line));
	block.path = _peek().value;
	_eat();
	_expect(TOKEN_LBRACE, "{");
	while (!_eof() && _peek().type != TOKEN_RBRACE)
	{
		const Token	&token = _peek();
		if (token.type == TOKEN_WORD)
			block.directives.push_back(_parseDirective());
		else
			throw std::runtime_error("Unexpected token inside location block on line "
				+ utils::toString(token.line));
	}
	_expect(TOKEN_RBRACE, "}");
	return (block);
}

ASTDirective	ConfigParser::_parseDirective()
{
	ASTDirective	directive;

	directive.name = _peek().value;
	directive.line = _peek().line;
	_eat();
	while (_peek().type == TOKEN_WORD)
	{
		directive.args.push_back(_peek().value);
		_eat();
	}
	_expect(TOKEN_SEMICOLON, ";");
	return (directive);
}
