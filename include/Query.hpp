/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/08 23:40:27 by victorviter      ###   ########.fr       */
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
		Query(Config *config, Client *client, std::map<std::string, Cookie *> *all_cookies);
		Query(const Query &other);
		Query &operator=(const Query &other);
	//DESTUCTORS
		~Query();
	//GETTERS
	//SETTERS
	//MEMBER FUNCTIONS
		int			queryRespond();
		int			queryGet();
		int			queryPost();
		int			queryDelete();
		int			queryCGIRun();
		int			queryError();
		
		int			readRequest();
		void		screenErrors();
		int			setRessource();
		int			findRessource();
		int			setRessourceStatus();
		std::string	getRessourceTypeStr();
		std::string	getRessourceTypeExtenssion();
		void		setHeader();
		int			sendHeader();
		int			streamFile(std::string file);
	private :
		static const int				_method_num = 5;
		typedef int						(Query::*queryMethod)();
		static const queryMethod		_queryExecute[_method_num];
		std::string						_query_str;
		Request							*_query;
		int								_err_code;
		std::string						_ressource;
		int								_ressource_status;
		std::string						_header;
		unsigned long					_content_len;
		ContentTypes					_content_type;
		Config							*_config;
		Client							*_client;
		std::map<std::string, Cookie *>	*_all_cookies;
		std::vector<Cookie *>			_query_cookies;
};
