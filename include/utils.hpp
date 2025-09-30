/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 18:05:38 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 14:02:39 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"

bool						endsWith(const std::string& str, const std::string& suffix);
std::vector<std::string>	stringSplit(std::string s, std::string d);