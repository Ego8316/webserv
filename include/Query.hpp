/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/09 18:29:59 by victorviter      ###   ########.fr       */
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

class Query
{
	public:
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
		static const int			_method_num = 5;
		typedef int					(Query::*queryMethod)(void);
		static const queryMethod	_queryExecute[_method_num];
		std::string					_request_str;
		Request						*_query;
		HttpStatus					_err_code;
		std::string					_resource;
		int							_resourceStatus;
		std::string					_header;
		unsigned long				_content_len;
		ContentTypes				_content_type;	//TODO implement setting this as part of setRessourceStatus
		Config						*_config;
		Client						*_client;
		Cookie						*_cookie;
};
