/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 18:05:02 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/02 21:10:58 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

bool endsWith(const std::string& str, const std::string& suffix)
{
	if (str.length() < suffix.length()) {
		return false;
	}
	return str.rfind(suffix) == (str.length() - suffix.length());
}

std::vector<std::string>		stringSplit(std::string s, std::string d)
{
	std::vector<std::string>	split_str;
	size_t						pos = 0;
	std::string					element;

	pos = s.find(d);
	while (pos != std::string::npos)
	{
        element = s.substr(0, pos);
        split_str.push_back(element);
        s.erase(0, pos + d.length());
		pos = s.find(d);
	}
	split_str.push_back(s);
	return (split_str);
}
