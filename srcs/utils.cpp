/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 18:05:02 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/13 22:47:03 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

bool utils::endsWith(const std::string& str, const std::string& suffix)
{
	if (str.length() < suffix.length())
	{
		return false;
	}
	return (str.substr(str.length() - suffix.length(), suffix.length()) == suffix);
}

bool utils::startsWith(const std::string& str, const std::string& prefix)
{
	if (str.length() < prefix.length())
	{
		return false;
	}
	return (str.substr(0, prefix.length()) == prefix);
}

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

std::string			utils::stringTrim(std::string str, std::string set)
{
	unsigned int	last_size = str.size() + 1;

	while (last_size != str.size())
	{
		last_size = str.size();
		if (set.find(str[0]) != std::string::npos)
			str.erase(0, 1);
		if (set.find(str[str.size() - 1]) != std::string::npos)
			str.erase(str.size() - 1, 1);
	}
	return (str);
}

size_t	utils::getFileSize(const std::string &path)
{
	struct stat	st;
	if (stat(path.c_str(), &st) == 0)
		return (st.st_size);
	return (0);
}

ContentTypes		utils::strToContentType(std::string input)
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
std::string	utils::contentTypeToStr(ContentTypes type)
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
ContentTypes		utils::extensionToContentTypes(std::string fname)
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
std::string	utils::contentTypeToExtensions(ContentTypes type)
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


std::string		utils::methodToStr(Method method)
{
	if (method == GET)
		return ("GET");
	else if (method == POST)
		return ("POST");
	else
		return ("DELETE");
	return ("");
}

Method			utils::strToMethod(std::string method_str)
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
 * @brief Get the file prefix of the full path
 * 
 * @return File prefix : if input is /some/directory/file the return value will be /some/directory
 * TODO ? remove ? not used anymore
 */
std::string		utils::extractPath(std::string full_path)
{
	if (full_path.find("/") == std::string::npos)
		return ("");
	else if (full_path.rfind("/") == full_path.size() - 1)
		return (full_path);
	return (full_path.substr(0, full_path.rfind("/")));
}
