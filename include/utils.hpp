/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 18:05:38 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/20 19:54:03 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"

namespace	utils
{

bool						endsWith(const std::string& str, const std::string& suffix);
bool 						startsWith(const std::string& str, const std::string& prefix);
std::vector<std::string>	stringSplit(std::string str, std::string del);
std::string					stringTrim(std::string str, std::string set);
std::string					toLower(const std::string &str);
std::string					capitalize(const std::string &str);
size_t						getFileSize(const std::string &path);
ContentTypes				strToContentType(std::string input);
std::string					contentTypeToStr(ContentTypes type);
ContentTypes				extensionToContentTypes(std::string fname);
std::string					contentTypeToExtensions(ContentTypes type);
std::string					methodToStr(Method method);
Method						strToMethod(std::string method);
long						getTime();

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
