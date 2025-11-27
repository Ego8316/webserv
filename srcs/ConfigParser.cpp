/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:42 by ego               #+#    #+#             */
/*   Updated: 2025/11/27 16:23:06 by ego              ###   ########.fr       */
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
 * @throws UnexpectedTokenError when a root-level token is not a `server` block.
 * @return Vector of parsed Block objects representing each server block.
 */
std::vector<Block>	ConfigParser::parse()
{
	std::vector<Block>	servers;

	while (!_eof())
	{
		const Token	&token = _peek();
		if (token.type == TOKEN_WORD && token.value == "server")
			servers.push_back(_parseServerBlock());
		else if (token.type == TOKEN_EOF)
			break ;
		else
			throw UnexpectedTokenError(token.line, token.value, "server");
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
 * @throws UnexpectedTokenError when called at EOF.
 *
 * @return Reference to the consumed Token.
 */
const Token	&ConfigParser::_eat()
{
	if (_eof())
		throw UnexpectedTokenError(_peek().line, "EOF", "more tokens");
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
 * @throws UnexpectedTokenError when the current token does not match `type`.
 *
 * @param type Expected TokenType.
 * @param expected Description used in error messages.
 */
void	ConfigParser::_expect(TokenType type, const std::string &expected)
{
	if (_peek().type != type)
		throw UnexpectedTokenError(_peek().line, _peek().value, expected);
	_eat();
	return ;
}

/**
 * @brief Parse a `server { ... }` block into an Block.
 *
 * Handles nested `location` blocks and plain directives.
 * 
 * @throws UnexpectedTokenError on unexpected items inside the block.
 *
 * @return Block representing this server block.
 */
Block	ConfigParser::_parseServerBlock()
{
	Block	block;

	block.type = "server";
	block.path = "";
	block.line = _peek().line;
	_expect(TOKEN_WORD, "server");
	_expect(TOKEN_LBRACE, "{");
	while (!_eof() && _peek().type != TOKEN_RBRACE)
	{
		const Token	&token = _peek();
		if (token.type == TOKEN_WORD && token.value == "error_pages")
			block.children.push_back(_parseErrorPagesBlock());
		else if (token.type == TOKEN_WORD && token.value == "location")
			block.children.push_back(_parseLocationBlock());
		else if (token.type == TOKEN_WORD)
			block.directives.push_back(_parseDirective());
		else
			throw UnexpectedTokenError(token.line, token.value, "directive or 'location'");
	}
	_expect(TOKEN_RBRACE, "}");
	return (block);
}

/**
 * @brief Parse a `error_pages { ... }` block at server scope.
 *
 * Expected shape: `error_pages { 404 /foo; 500 /bar; }`
 * Only server scope is allowed.
 *
 * @throws UnexpectedTokenError on malformed block contents.
 */
Block	ConfigParser::_parseErrorPagesBlock()
{
	Block		block;
	const int	start_line = _peek().line;

	block.type = "error_pages";
	block.line = start_line;
	block.path = "";
	_expect(TOKEN_WORD, "error_pages");
	_expect(TOKEN_LBRACE, "{");
	while (!_eof() && _peek().type != TOKEN_RBRACE)
	{
		const Token	&token = _peek();
		if (token.type == TOKEN_WORD)
			block.directives.push_back(_parseDirective());
		else
			throw UnexpectedTokenError(token.line, token.value, "directive");
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
 * @throws UnexpectedTokenError on missing path or bad contents.
 * 
 * @return Block representing the location block.
 */
Block	ConfigParser::_parseLocationBlock()
{
	Block	block;

	block.type = "location";
	block.line = _peek().line;
	_expect(TOKEN_WORD, "location");
	if (_peek().type != TOKEN_WORD)
		throw UnexpectedTokenError(_peek().line, _peek().value, "path");
	block.path = _peek().value;
	_eat();
	_expect(TOKEN_LBRACE, "{");
	while (!_eof() && _peek().type != TOKEN_RBRACE)
	{
		const Token	&token = _peek();
		if (token.type == TOKEN_WORD)
			block.directives.push_back(_parseDirective());
		else
			throw UnexpectedTokenError(token.line, token.value, "directive");
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
 * @throws UnexpectedTokenError on bad token sequence or missing ';'.
 *
 * @return Directive containing the directive name and its arguments.
 */
Directive	ConfigParser::_parseDirective()
{
	Directive	directive;

	directive.name = _peek().value;
	directive.line = _peek().line;
	_eat();
	while (_peek().type == TOKEN_WORD)
	{
		directive.args.push_back(_peek().value);
		_eat();
	}
	if (_peek().line != directive.line)
		throw UnexpectedTokenError(directive.line, _peek().value, ";");
	_expect(TOKEN_SEMICOLON, ";");
	return (directive);
}
