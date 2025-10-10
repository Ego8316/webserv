/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 18:05:02 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/10 14:46:13 by victorviter      ###   ########.fr       */
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

std::string					utils::stringTrim(std::string str, std::string set)
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
