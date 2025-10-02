/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/02 21:28:12 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "utils.hpp"
#include "Config.hpp"
#include "Cookie.hpp"

class Client;
class Request;
class Config;
class Cookie;

class Query {
	public :
	// CONSTRUCTORS
		Query(Config *config, Client *client);
		Query(const Query &other);
		Query &operator=(const Query &other);
		~Query(void);

		int			queryRespond(void);
		int			queryGet(void);
		int			queryPost(void);
		int			queryDelete(void);
		int			queryCGIRun(void);
		int			queryError(void);
		
		int			readRequest();
		int			setCookie();
		int			setRessource();
		int			findRessource();
		int			setRessourceStatus();
		std::string	getRessourceTypeStr();
		std::string	getRessourceTypeExtenssion();
		void		setHeader();
		int			sendHeader();
		int			streamFile(std::string file);
		std::string	httpStatusToStr(HttpStatus code);
		std::string	getDefaultErrorPage(HttpStatus code);

	private:
		static const int			_methodNum = 5;
		typedef int					(Query::*queryMethod)(void);
		static const queryMethod	_queryExecute[_methodNum];
		std::string					_requestStr;
		Request						*_query;
		HttpStatus					_statusCode;
		std::string					_ressource;
		int							_ressourceStatus;
		std::string					_header;
		unsigned long				_contentLen;
		ContentTypes				_contentType;	//TODO implement setting this as part of setRessourceStatus
		Config						*_config;
		Client						*_client;
		Cookie						*_cookie;
};
