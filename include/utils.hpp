/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vviterbo <vviterbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 18:05:38 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/02 13:15:40 by vviterbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"

class Client;
namespace	utils
{

bool						endsWith(const std::string& str, const std::string& suffix);
bool 						startsWith(const std::string& str, const std::string& prefix);
std::vector<std::string>	stringSplit(std::string str, std::string del);
std::string					stringTrim(std::string &str, const std::string &set);
std::string					stringTrimSpaces(std::string &str);
std::string					toLower(const std::string &str);
std::string					toUpper(const std::string &str);
std::string					capitalize(const std::string &str);
std::string::iterator		caseInsensitiveFind(std::string &haystack, std::string needle);
ssize_t						getFileSize(const std::string &path);
ContentType					strToContentType(std::string input);
std::string					contentTypeToStr(ContentType type);
ContentType					extensionToContentTypes(std::string fname);
std::string					contentTypeToExtensions(ContentType type);
std::string					methodToStr(Method method);
Method						strToMethod(const std::string &method);
std::string					httpStatusToStr(HttpStatus code);
HttpStatus					strToHttpStatus(std::string status);
std::string					stateToStr(RequestStage state);
long						getTime();
std::string					timeStamp();
void						logMsg(const std::string &level, const std::string &color, const std::string &msg, int client_id = -1);
void						printProcessQueue(std::deque<Client *> q);
void						printClients(std::vector<Client *> v);

template <typename T>
std::string	toString(const T &src)
{
	std::ostringstream  oss;
	oss << src;
	return (oss.str());
}

template <typename K, typename V>
bool	mapHasEntry(const std::map<K, V> &map, const K &key)
{
	return (map.find(key) != map.end());
}

}
