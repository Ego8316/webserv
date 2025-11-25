/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 18:05:02 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/25 00:44:16 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

/**
 * @brief Returns true when `str` ends with the given suffix.
 *
 * @param str String to test.
 * @param suffix Trailing substring to match.
 *
 * @return True when suffix matches.
 */
bool	utils::endsWith(const std::string& str, const std::string& suffix)
{
	if (str.length() < suffix.length())
	{
		return false;
	}
	return (str.substr(str.length() - suffix.length(), suffix.length()) == suffix);
}

/**
 * @brief Returns true when `str` starts with the given prefix.
 *
 * @param str String to test.
 * @param prefix Leading substring to match.
 *
 * @return True when prefix matches.
 */
bool	utils::startsWith(const std::string& str, const std::string& prefix)
{
	if (str.length() < prefix.length())
	{
		return false;
	}
	return (str.substr(0, prefix.length()) == prefix);
}

/**
 * @brief Splits a string on a delimiter substring.
 *
 * @param str Input string.
 * @param del Delimiter substring.
 *
 * @return Vector of split parts.
 */
std::vector<std::string>	utils::stringSplit(std::string str, std::string del)
{
	std::vector<std::string>	split_str;
	size_t						pos = 0;
	std::string					element;

	pos = str.find(del);
	while (pos != std::string::npos)
	{
		element = str.substr(0, pos);
		split_str.push_back(element);
		str.erase(0, pos + del.length());
		pos = str.find(del);
	}
	split_str.push_back(str);
	return (split_str);
}

/**
 * @brief Trims characters in `set` from both ends of the string.
 *
 * @param str String to trim (modified in place).
 * @param set Characters to remove.
 *
 * @return Trimmed string.
 */
std::string	utils::stringTrim(std::string &str, const std::string &set)
{
	unsigned int	last_size = str.size() + 1;

	while (last_size != str.size())
	{
		last_size = str.size();
		if (!str.empty() && set.find(str[0]) != std::string::npos)
			str.erase(0, 1);
		if (!str.empty() && set.find(str[str.size() - 1]) != std::string::npos)
			str.erase(str.size() - 1, 1);
	}
	return (str);
}

/**
 * @brief Trims whitespace and newline characters from both ends.
 *
 * @param str String to trim (modified in place).
 *
 * @return Trimmed string.
 */
std::string	utils::stringTrimSpaces(std::string &str)
{
	return (stringTrim(str, " \r\n\t"));
}

/**
 * @brief Converts a string to lowercase.
 *
 * @param str Input string.
 *
 * @return Lowercased copy.
 */
std::string	utils::toLower(const std::string &str)
{
	std::string	lower = str;

	for (size_t i = 0; i < lower.size(); ++i)
		lower[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lower[i])));
	return (lower);
}

/**
 * @brief Converts a string to uppercase.
 *
 * @param str Input string.
 *
 * @return Uppercased copy.
 */
std::string	utils::toUpper(const std::string &str)
{
	std::string	upper = str;

	for (size_t i = 0; i < upper.size(); ++i)
		upper[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(upper[i])));
	return (upper);
}

/**
 * @brief Capitalizes dash-separated words and lowercases the rest.
 *
 * @param str Input string.
 *
 * @return Capitalized copy.
 */
std::string	utils::capitalize(const std::string &str)
{
	std::string	result = str;
	bool		new_word = true;

	for (size_t i = 0; i < result.size(); ++i)
	{
		if (new_word && std::isalpha(static_cast<unsigned char>(result[i])))
		{
			result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
			new_word = false;
		}
		else
			result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
		if (result[i] == '-')
			new_word = true;
	}
	return (result);
}

/**
 * @brief no fucking clue
 */
static bool	CICharComp(char a, char b)
{
	return (std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b)));
}

/**
 * @brief Case-insensitive search of `needle` within `haystack`.
 *
 * @param haystack String to search.
 * @param needle Substring to locate.
 *
 * @return Iterator to first match or end iterator.
 */
std::string::iterator	utils::caseInsensitiveFind(std::string &haystack, std::string needle)
{
	std::string::iterator it = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(), CICharComp);
	return (it);
}

/**
 * @brief Returns file size in bytes or -1 on error.
 *
 * @param path Filesystem path.
 *
 * @return Size in bytes or -1.
 */
ssize_t	utils::getFileSize(const std::string &path)
{
	struct stat	st;

	if (stat(path.c_str(), &st) == 0)
		return (st.st_size);
	return (-1);
}

/**
 * @brief Converts a MIME string (e.g., text/html) to ContentType flags.
 *
 * @param input MIME string.
 *
 * @return ContentType bitmask.
 */
ContentType	utils::strToContentType(std::string input)
{
	if (input == "*/*")
		return (FTYPE_ANY);
	if (input == "text/*")
		return (FTYPE_TEXT);
	if (input == "text/plain")
		return (FTYPE_PLAIN);
	if (input == "image/*")
		return (FTYPE_IMAGE);
	if (input == "text/html")
		return (FTYPE_HTML);
	if (input == "image/png")
		return (FTYPE_PNG);
	if (input == "image/jpeg")
		return (FTYPE_JPEG);
	return (FTYPE_NONE);
}

/**
 * @brief Get MIME type string based on detected content type.
 *
 * @return The MIME type (e.g., "text/html", "image/png").
 */
std::string	utils::contentTypeToStr(ContentType type)
{
	switch(type)
	{
		case FTYPE_HTML:	return "text/html";
		case FTYPE_PLAIN:	return "text/plain";
		case FTYPE_JPEG:	return "image/jpeg";
		case FTYPE_PNG:		return "image/png";
		default:			return "";
	}
}

/**
 * @brief Detects the type of the resource based on the file extension.
 *
 * Updates _status to include IS_CGI for Python and PHP scripts.
 */
ContentType	utils::extensionToContentTypes(std::string fname)
{
	if (utils::endsWith(fname, ".py"))
		return (FTYPE_CGI_PY);
	else if (utils::endsWith(fname, ".php"))
		return (FTYPE_CGI_PHP);
	else if (utils::endsWith(fname, ".html"))
		return (FTYPE_HTML);
	else if (utils::endsWith(fname, ".jpeg"))
		return (FTYPE_JPEG);
	else if (utils::endsWith(fname, ".png"))
		return (FTYPE_PNG);
	else
		return (FTYPE_PLAIN);
}

/**
 * @brief Get the file extension associated with the content type.
 *
 * @return File extension (e.g., ".html", ".png", ".php").
 */
std::string	utils::contentTypeToExtensions(ContentType type)
{
	switch(type)
	{
		case FTYPE_HTML:		return ".html";
		case FTYPE_JPEG:		return ".jpeg";
		case FTYPE_PNG:			return ".png";
		case FTYPE_CGI_PY:		return ".py";
		case FTYPE_CGI_PHP:		return ".php";
		default:				return "";
	}
}

/**
 * @brief Converts an HTTP method enum to its string form.
 *
 * @param method HTTP method enum.
 *
 * @return Method name.
 */
std::string	utils::methodToStr(Method method)
{
	switch(method)
	{
		case GET:		return ("GET");
		case POST:		return ("POST");
		case DELETE:	return ("DELETE");
		default:		return ("UNKNOWN");
	}
}

/**
 * @brief Converts a method string to its Method enum value.
 *
 * @param method_str Method name.
 *
 * @return Method enum value or UNKNOWN.
 */
Method	utils::strToMethod(const std::string &method_str)
{
	if (method_str == "GET")
		return (GET);
	else if (method_str == "POST")
		return (POST);
	else if (method_str == "DELETE")
		return (DELETE);
	return (UNKNOWN);
}

/**
 * @brief Converts an HTTP status code enum to its corresponding string.
 *
 * @param code The HTTP status code.
 *
 * @return `std::string` The string representation of the code.
 */
std::string	utils::httpStatusToStr(HttpStatus code)
{
	switch(code)
	{
		case HTTP_OK:						return "OK";
		case HTTP_BAD_REQUEST:				return "Bad Request";
		case HTTP_FORBIDDEN:				return "Forbidden";
		case HTTP_NOT_FOUND:				return "Not Found";
		case HTTP_INTERNAL_SERVER_ERROR:	return "Internal Server Error";
		case HTTP_NOT_IMPLEMENTED:			return "Not Implemented";
		case HTTP_VERSION_NOT_SUPPORTED:	return "HTTP Version Not Supported";
		default:							return "Unknown";
	}
}

/**
 * @brief Converts an HTTP status code str to its corresponding enum.
 *
 * @param code The HTTP status code.
 *
 * @return `std::string` The string representation of the code.
 */
HttpStatus	utils::strToHttpStatus(std::string status)
{
	if (utils::startsWith(status, "200"))
		return (HTTP_OK);
	if (utils::startsWith(status, "201"))
		return (HTTP_CREATED);
	if (utils::startsWith(status, "202"))
		return (HTTP_ACCEPTED);
	if (utils::startsWith(status, "204"))
		return (HTTP_NO_CONTENT);
	if (utils::startsWith(status, "300"))
		return (HTTP_REDIRECT);
	if (utils::startsWith(status, "301"))
		return (HTTP_REDIRECT_PERM);
	if (utils::startsWith(status, "302"))
		return (HTTP_REDIRECT_TEMP);
	if (utils::startsWith(status, "400"))
		return (HTTP_BAD_REQUEST);
	if (utils::startsWith(status, "401"))
		return (HTTP_UNAUTHORIZED);
	if (utils::startsWith(status, "403"))
		return (HTTP_FORBIDDEN);
	if (utils::startsWith(status, "404"))
		return (HTTP_NOT_FOUND);
	if (utils::startsWith(status, "409"))
		return (HTTP_CONFLICT);
	if (utils::startsWith(status, "500"))
		return (HTTP_INTERNAL_SERVER_ERROR);
	if (utils::startsWith(status, "501"))
		return (HTTP_NOT_IMPLEMENTED);
	if (utils::startsWith(status, "502"))
		return (HTTP_BAD_GATEWAY);
	if (utils::startsWith(status, "505"))
		return (HTTP_VERSION_NOT_SUPPORTED);
	return (HTTP_UNKNOWN_STATUS);
}

/**
 * @brief Converts a RequestStage value to a readable label.
 *
 * @param state Request stage.
 *
 * @return Stage name.
 */
std::string	utils::stateToStr(RequestStage state)
{
	switch (state)
	{
		case TRY_ACCEPTING:			return "TRY_ACCEPTING";
		case INIT:					return "INIT";
		case READING_HEADER:		return "READING_HEADER";
		case READING_BODY:			return "READING_BODY";
		case PROCESSING_REQUEST:	return "PROCESSING_REQUEST";
		case CGI_RUNNING:			return "CGI_RUNNING";
		case SENDING_STRING:		return "SENDING_STRING";
		case SENDING_FILE:			return "SENDING_FILE";
		case ABORTING:				return "ABORTING";
		case DONE:					return "DONE";
		default:					return "UNKNOWN_STATE";
	}
}

/**
 * @brief Returns the current epoch time (seconds).
 *
 * @return Epoch time or 0 on error.
 */
long	utils::getTime()
{
	time_t	t;
	
	t = time(NULL);
	if (t == -1)
	{
		std::cerr << "Clock error. Seting time to 0" << std::endl;
		return (0);
	}
	return (static_cast<long>(t));
}
