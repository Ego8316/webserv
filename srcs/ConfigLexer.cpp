/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:35 by ego               #+#    #+#             */
/*   Updated: 2025/11/26 20:05:53 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigLexer.hpp"

/**
 * @brief Construct a lexer bound to the given configuration text.
 * 
 * @param input Complete configuration file content.
 */
ConfigLexer::ConfigLexer(const std::string &input)
	:	_input(input),
		_pos(0),
		_line(1)
{
	return ;
}

/**
 * @brief Convert the raw configuration text into a sequence of tokens.
 * 
 * @return Vector containing the full token stream.
 */
std::vector<Token>	ConfigLexer::tokenize()
{
	std::vector<Token>	tokens;

	while (true)
	{
		Token	token = _getNextToken();
		tokens.push_back(token);
		if (token.type == TOKEN_EOF)
			break ;
	}
	return (tokens);
}

/**
 * @brief Checks wheter the end of input has been reached.
 * 
 * @return true if the cursor has passed the final character, false otherwise.
 */
bool	ConfigLexer::_eof() const
{
	return (_pos >= _input.size());
}

/**
 * @brief Peek at the current character without consuming it.
 * 
 * @return The current character, or '\0' on EOF.
 */
char	ConfigLexer::_peek() const
{
	return (_eof() ? '\0' : _input[_pos]);
}

/**
 * @brief Consume and return the current character.
 * 
 * @return The consumed character, or '\0' on EOF.
 */
char	ConfigLexer::_get()
{
	if (_eof()) return ('\0');
	char	c = _input[_pos++];
	if (c == '\n') _line++;
	return (c);
}

/**
 * @brief Skips over whitespaces and '#' comments until a real token begins.
 */
void	ConfigLexer::_skipJunk()
{
	while (!_eof())
	{
		while (!_eof() && isspace(_peek()))
			_get();
		if (!_eof() && _peek() == '#')
		{
			while (!_eof() && _peek() != '\n')
				_get();
			continue ;
		}
		break ;
	}
}

/**
 * @brief Creates a token of the given type, value and line number.
 * 
 * @param type Token type.
 * @param value Raw token text.
 * @param line Line on which the token was read.
 * 
 * @return The newly constructed token.
 */
Token	ConfigLexer::_newToken(TokenType type, const std::string &value, int line) const
{
	Token	token;

	token.type = type;
	token.value = value;
	token.line = line;
	return (token);
}

/**
 * @brief Read a continuouss word token.
 * 
 * A word stops when encountering whitespace, '{', '}', ';' or '#'.
 * 
 * @return The token representing the extracted word.
 */
Token	ConfigLexer::_readWord()
{
	std::string	value;
	int			line = _line;

	while (!_eof())
	{
		char	c = _peek();
		if (isspace(c) || c == '{' || c == '}' || c == ';' || c == '#')
			break ;
		value += _get();
	}
	return (_newToken(TOKEN_WORD, value, line));
}

/**
 * @brief Extract the next token from input.
 * 
 * Skips leading junk, then returns braces, semicolons, or a word token.
 * 
 * @return The next token in the stream.
 */
Token	ConfigLexer::_getNextToken()
{
	_skipJunk();
	if (_eof())
		return (_newToken(TOKEN_EOF, "", _line));
	char	c = _peek();
	int		line = _line;
	if (c == '{') return (_get(), _newToken(TOKEN_LBRACE, "{", line));
	if (c == '}') return (_get(), _newToken(TOKEN_RBRACE, "}", line));
	if (c == ';') return (_get(), _newToken(TOKEN_SEMICOLON, ";", line));
	return (_readWord());
}
