/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:35 by ego               #+#    #+#             */
/*   Updated: 2025/11/26 13:55:34 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigLexer.hpp"

/**
 * @brief Parametric constructor.
 * 
 * @param input The whole configuration file as a string.
 */
ConfigLexer::ConfigLexer(const std::string &input)
	:	_input(input),
		_pos(0),
		_line(1)
{
	return ;
}

/**
 * @brief Goes through the input and tokenizes it.
 * 
 * @return The configuration file as a token vector.
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
 * @brief Tells whether end of file has been reached yet.
 * 
 * @return true if end of file hass been reached, false otherwise.
 */
bool	ConfigLexer::_eof() const
{
	return (_pos >= _input.size());
}

/**
 * @brief Gets the next current character without updating position nor
 * line number.
 * 
 * @return The current character.
 */
char	ConfigLexer::_peek() const
{
	return (_eof() ? '\0' : _input[_pos]);
}

/**
 * @brief Gets the next character. Updates position and line number.
 * 
 * @return The next character.
 */
char	ConfigLexer::_get()
{
	if (_eof()) return ('\0');
	char	c = _input[_pos++];
	if (c == '\n') _line++;
	return (c);
}

/**
 * @brief Skips whitespace characters and comments starting by `#` until a
 * token is reached.
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
 * @brief Builds a new token with the parameters given.
 * 
 * @param type Token type.
 * @param value Token value.
 * @param line Token line number.
 * 
 * @return The freshly born token.
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
 * @brief Reads input until end of current word is reached and builds a token
 * out of it.
 * 
 * @return The tokenized word.
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
 * @brief Gets the next token in file by first skipping junk.
 * 
 * @return The next token in file.
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
