/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:10:23 by ego               #+#    #+#             */
/*   Updated: 2025/11/27 02:39:54 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"
#include "ConfigLexer.hpp"
#include "ConfigParser.hpp"

std::string	tokenTypeToStr(TokenType t);
void		printTokens(const std::vector<Token> &tokens);
void		printBlocks(const std::vector<Block> &blocks);


std::ostream	&operator<<(std::ostream &os, TokenType t);
std::ostream	&operator<<(std::ostream &os, Token t);
std::ostream	&operator<<(std::ostream &os, const std::vector<Token> &tokens);
