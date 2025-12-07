/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 18:05:02 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/07 19:38:27 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include <sys/time.h>

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
		return (false);
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
		return (false);
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
 * @brief case insensitive char comparison
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
	if (input.empty())
		return (FTYPE_NONE);
	if (input == "*/*")
		return (FTYPE_ANY);
	if (input == "text/*")
		return (FTYPE_TEXT);
	if (input == "text/plain")
		return (FTYPE_PLAIN);
	if (input == "text/css")
		return (FTYPE_CSS);
	if (input == "image/*")
		return (FTYPE_IMAGE);
	if (input == "text/html")
		return (FTYPE_HTML);
	if (input == "image/png")
		return (FTYPE_PNG);
	if (input == "image/jpeg")
		return (FTYPE_JPEG);
	if (input == "image/jpg")
		return (FTYPE_JPEG);
	if (input == "image/svg+xml")
		return (FTYPE_SVG);
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
		case FTYPE_TEXT:	return "text/plain";
		case FTYPE_CSS:		return "text/css";
		case FTYPE_JPEG:	return "image/jpeg";
		case FTYPE_PNG:		return "image/png";
		case FTYPE_SVG:		return "image/svg+xml";
		case FTYPE_IMAGE:	return "image/*";
		default:			return "";
	}
}

/**
 * @brief Detects the type of the resource based on the file extension.
 *
 * Updates _status to include IS_CGI for Python and Perl scripts.
 */
ContentType	utils::extensionToContentTypes(std::string fname)
{
	if (utils::endsWith(fname, ".py"))
		return (FTYPE_CGI_PY);
	else if (utils::endsWith(fname, ".pl"))
		return (FTYPE_CGI_PL);
	else if (utils::endsWith(fname, ".html"))
		return (FTYPE_HTML);
	else if (utils::endsWith(fname, ".css"))
		return (FTYPE_CSS);
	else if (utils::endsWith(fname, ".jpeg"))
		return (FTYPE_JPEG);
	else if (utils::endsWith(fname, ".jpg"))
		return (FTYPE_JPEG);
	else if (utils::endsWith(fname, ".png"))
		return (FTYPE_PNG);
	else if (utils::endsWith(fname, ".svg"))
		return (FTYPE_SVG);
	else
		return (FTYPE_PLAIN);
}

/**
 * @brief Get the file extension associated with the content type.
 *
 * @return File extension (e.g., ".html", ".png", ".pl").
 */
std::string	utils::contentTypeToExtensions(ContentType type)
{
	switch(type)
	{
		case FTYPE_HTML:		return ".html";
		case FTYPE_PLAIN:		return ".txt";
		case FTYPE_CSS:			return ".css";
		case FTYPE_JPEG:		return ".jpeg";
		case FTYPE_PNG:			return ".png";
		case FTYPE_SVG:			return ".svg";
		case FTYPE_CGI_PY:		return ".py";
		case FTYPE_CGI_PL:		return ".pl";
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
		case HTTP_CREATED:					return "Created";
		case HTTP_NO_CONTENT:				return "No Content";
		case HTTP_REDIRECT_PERM:			return "Moved Permanently";
		case HTTP_REDIRECT_TEMP:			return "Found";
		case HTTP_BAD_REQUEST:				return "Bad Request";
		case HTTP_FORBIDDEN:				return "Forbidden";
		case HTTP_NOT_FOUND:				return "Not Found";
		case HTTP_METHOD_NOT_ALLOWED:		return "Method Not Allowed";
		case HTTP_NOT_ACCEPTABLE:			return "Not Acceptable";
		case HTTP_TIMEOUT:					return "Request Timeout";
		case HTTP_CONFLICT:					return "Conflict";
		case HTTP_CONTENT_TOO_LARGE:		return "Payload Too Large";
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
	if (status.size() < 3)
		return (HTTP_UNKNOWN_STATUS);
	int	code = std::atoi(status.c_str());
	switch (code)
	{
		case 200:	return HTTP_OK;
		case 201:	return HTTP_CREATED;
		case 204:	return HTTP_NO_CONTENT;
		case 301:	return HTTP_REDIRECT_PERM;
		case 302:	return HTTP_REDIRECT_TEMP;
		case 400:	return HTTP_BAD_REQUEST;
		case 403:	return HTTP_FORBIDDEN;
		case 404:	return HTTP_NOT_FOUND;
		case 405:	return HTTP_METHOD_NOT_ALLOWED;
		case 406:	return HTTP_NOT_ACCEPTABLE;
		case 408:	return HTTP_TIMEOUT;
		case 409:	return HTTP_CONFLICT;
		case 413:	return HTTP_CONTENT_TOO_LARGE;
		case 500:	return HTTP_INTERNAL_SERVER_ERROR;
		case 501:	return HTTP_NOT_IMPLEMENTED;
		case 505:	return HTTP_VERSION_NOT_SUPPORTED;
		default:	break;
	}
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
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000);
}

/**
 * @brief Build a timestamp string.
 *
 * @return Timestamp in HH:MM:SS format.
 */
std::string	utils::timeStamp()
{
	struct timeval	tv;
	struct tm		tm;
	char			buffer[32];

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);
	std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
	return (std::string(buffer));
}

/**
 * @brief Log a colored message with timestamp and optional client id.
 *
 * @param level  Textual level label.
 * @param color  ANSI color prefix to apply.
 * @param msg    Message body.
 * @param client_id Client identifier (-1 to skip).
 */
void	utils::logMsg(const std::string &level, const std::string &color, const std::string &msg, int client_id)
{
	std::cout << color << "[" << timeStamp() << "] [" << level << "]";
	if (client_id >= 0)
		std::cout << " [client " << client_id << "]";
	std::cout << " " << msg << RESET << std::endl;
}

void	utils::printProcessQueue(std::deque<Client *> &q)
{
	std::deque<Client *>::iterator	it = q.begin();
	size_t	i = 0;

	std::cout << "Processing Queue" << std::endl;
	while (it != q.end() && i < 10)
	{
		std::cout << "PQ[" << i << "]" << " = " << *it << std::endl;
		++it;
		++i;
	}
}

void	utils::printClients(std::vector<Client *> &v)
{
	std::vector<Client *>::iterator	it = v.begin();
	size_t	i = 0;

	std::cout << "Clients" << std::endl;
	while (it != v.end() && i < 10)
	{
		std::cout << "Clients[" << i << "]" << " = " << *it << std::endl;
		++it;
		++i;
	}
}