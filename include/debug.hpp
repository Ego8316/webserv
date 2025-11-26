/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:10:23 by ego               #+#    #+#             */
/*   Updated: 2025/11/26 14:12:05 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"
#include "ConfigLexer.hpp"

std::string	tokenTypeToStr(TokenType t);
void		printTokens(const std::vector<Token> &tokens);