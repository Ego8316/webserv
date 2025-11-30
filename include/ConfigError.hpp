/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigError.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 05:04:56 by ego               #+#    #+#             */
/*   Updated: 2025/11/27 17:47:05 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"

/**
 * @brief Base config error carrying a line number and formatted message.
 */
class	ConfigError	:	public	std::runtime_error
{
	public:
		ConfigError(int line, const std::string &msg)
			: std::runtime_error(_format(line, msg)), _line(line)
		{}

		int	line() const { return (_line); }

	private:
		int	_line;

		static std::string	_format(int line, const std::string &msg)
		{
			std::ostringstream	oss;

			oss << msg << " at line " << line;
			return (oss.str());
		}
};

/**
 * @brief Thrown when a token does not match what was expected.
 */
class	UnexpectedTokenError : public ConfigError
{
	public:
		UnexpectedTokenError(int line, const std::string &token, const std::string &expectation)
			: ConfigError(line, "Unexpected token '" + token + "', expected '" + expectation + "'")
		{}
};

/**
 * @brief Thrown on directives that are unknown in the current context.
 */
class	UnknownDirectiveError	:	public	ConfigError
{
	public:
		UnknownDirectiveError(int line, const std::string &name)
			: ConfigError(line, "Unknown directive '" + name + "'")
		{}
};

/**
 * @brief Thrown when an element (directive, location, etc.) is duplicated.
 */
class	DuplicateError	:	public	ConfigError
{
	public:
		DuplicateError(int line, const std::string &name)
			: ConfigError(line, "Duplicate '" + name + "'")
		{}
};

/**
 * @brief Thrown when a required directive argument is missing.
 */
class	MissingArgumentError	:	public	ConfigError
{
	public:
		MissingArgumentError(int line, const std::string &name)
			: ConfigError(line, "Missing argument for directive '" + name + "'")
		{}
};

/**
 * @brief Thrown when too many directive arguments are provided.
 */
class	TooManyArgumentsError	:	public ConfigError
{
	public:
		TooManyArgumentsError(int line, const std::string &name)
			: ConfigError(line, "Too many arguments for directive '" + name + "'")
		{}
};

/**
 * @brief Thrown for malformed size values.
 */
class	InvalidSizeError	:	public	ConfigError
{
	public:
		InvalidSizeError(int line, const std::string &value)
			: ConfigError(line, "Invalid size value '" + value + "'")
		{}
};

/**
 * @brief Thrown for bad size suffixes.
 */
class	InvalidSizeSuffixError	:	public ConfigError
{
	public:
		InvalidSizeSuffixError(int line, const std::string &value)
			: ConfigError(line, "Invalid size suffix in '" + value + "'")
		{}
};

/**
 * @brief Thrown when a size suffix is unrecognized.
 */
class	UnknownSizeError	:	public ConfigError
{
	public:
		UnknownSizeError(int line, const std::string &value)
			: ConfigError(line, "Unknown size suffix in '" + value + "'")
		{}
};

/**
 * @brief Thrown for unsupported HTTP methods.
 */
class	UnknownMethodError	:	public ConfigError
{
	public:
		UnknownMethodError(int line, const std::string &method)
			: ConfigError(line, "Unknown method '" + method + "'")
		{}
};

/**
 * @brief Base listen error for invalid endpoint strings.
 */
class	InvalidListenError	:	public ConfigError
{
	public:
		InvalidListenError(int line, const std::string &value)
			: ConfigError(line, "Invalid listen value '" + value + "'")
		{}
};

/**
 * @brief Thrown when listen has the wrong overall format.
 */
class	InvalidListenFormatError	:	public ConfigError
{
	public:
		InvalidListenFormatError(int line, const std::string &value)
			: ConfigError(line, "Invalid listen format '" + value + "'")
		{}
};

/**
 * @brief Thrown when the listen host is not a valid IPv4 address.
 */
class	InvalidListenHostError	:	public ConfigError
{
	public:
		InvalidListenHostError(int line, const std::string &value)
			: ConfigError(line, "Invalid listen host '" + value + "'")
		{}
};

/**
 * @brief Thrown when the listen port is malformed or out of range.
 */
class	InvalidListenPortError	:	public ConfigError
{
	public:
		InvalidListenPortError(int line, const std::string &value)
			: ConfigError(line, "Invalid listen port '" + value + "'")
		{}
};

/**
 * @brief Thrown when a status code token is malformed.
 */
class	InvalidStatusCodeError	:	public ConfigError
{
	public:
		InvalidStatusCodeError(int line, const std::string &value)
			: ConfigError(line, "Invalid status code '" + value + "'")
		{}
};

/**
 * @brief Thrown when a status code is well-formed but unsupported.
 */
class	UnknownStatusCodeError	:	public ConfigError
{
	public:
		UnknownStatusCodeError(int line, const std::string &value)
			: ConfigError(line, "Unknown status code '" + value + "'")
		{}
};
