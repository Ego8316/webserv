/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 18:05:38 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/07 18:24:04 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"

namespace	utils
{

bool						endsWith(const std::string& str, const std::string& suffix);
std::vector<std::string>	stringSplit(std::string s, std::string d);
size_t						getFileSize(const std::string &path);

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
