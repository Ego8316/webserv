/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:42 by ego               #+#    #+#             */
/*   Updated: 2025/11/26 17:18:02 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

/**
 * @brief Builds a ConfigParser object out of a token vector.
 */
ConfigParser::ConfigParser(const std::vector<Token> &tokens)
	:	_tokens(tokens),
		_pos(0)
{
	return ;
}

/**
 * @brief Parse the full token stream and return all top-level server blocks.
 *
 * Expects only `server { ... }` blocks at the root. Throws if any other token
 * appears at top level. Stops on TOKEN_EOF.
 *
 * @return Vector of parsed ASTBlock objects representing each server block.
 */
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

/**
 * @brief Peek at the current token without advancing the cursor.
 *
 * @return Reference to the current Token.
 */
const Token	&ConfigParser::_peek() const
{
	return (_tokens[_pos]);
}

/**
 * @brief Consume and return the current token, advancing the cursor.
 *
 * @throws std::runtime_error If called at EOF.
 *
 * @return Reference to the consumed Token.
 */
const Token	&ConfigParser::_eat()
{
	if (_eof())
		throw std::runtime_error("Unexpected end of file at line "
			+ _peek().line);
	return (_tokens[_pos++]);
}

/**
 * @brief Checks whether the parser has reached the end of token stream.
 *
 * TOKEN_EOF always marks the end of input.
 *
 * @return true if current token is TOKEN_EOF, false otherwise.
 */
bool	ConfigParser::_eof() const
{
	return	(_tokens[_pos].type == TOKEN_EOF);
}

/**
 * @brief Ensure that the current token matches the expected type.
 *
 * Consumes the token if correct. Throws on mismatch.
 * 
 * @throws std::runtime_error If unexpected token.
 *
 * @param type Expected TokenType.
 * @param expected Description used in error messages.
 */
void	ConfigParser::_expect(TokenType type, const std::string &expected)
{
	if (_peek().type != type)
		throw std::runtime_error("Unexpected token at line "
			+ utils::toString(_peek().line) + ": expected '" + expected + "'");
	_eat();
	return ;
}

/**
 * @brief Parse a `server { ... }` block into an ASTBlock.
 *
 * Handles nested `location` blocks and plain directives.
 * 
 * @throws std::runtime_error On unexpected tokens or malformed structure.
 *
 * @return ASTBlock representing this server block.
 */
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

/**
 * @brief Parse a `location <path> { ... }` block.
 *
 * Requires a path argument after 'location', then a `{`, followed by
 * directives until the matching `}`.
 *  
 * @throws std::runtime_error On unexpected tokens or malformed structure.
 * 
 * @return ASTBlock representing the location block.
 */
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

/**
 * @brief Parse a directive of the form:
 *        name arg1 arg2 ... ;
 *
 * All arguments and the terminating semicolon must appear on the same line.
 *
 * @throws std::runtime_error On newline inside the directive or missing ';'.
 *
 * @return ASTDirective containing the directive name and its arguments.
 */
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
	if (_peek().line != directive.line)
		throw std::runtime_error("Unexpected newline in directive '"
			+ directive.name + "' starting at line "
			+ utils::toString(directive.line));
	_expect(TOKEN_SEMICOLON, ";");
	return (directive);
}
