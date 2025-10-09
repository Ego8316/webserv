/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/09 22:03:47 by victorviter      ###   ########.fr       */
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
		int			queryRedirect();
		int			queryListDir();
		int			readRequest();
		int			screenErrors();
		
		int			setResource();
		int			findResource();
		void		checkRedirections();
		int			setResourceStatus();
		std::string	getResourceTypeStr();
		std::string	getResourceTypeExtenssion();
		void		setHeader();
		int			sendHeader();
		
		int			streamFile(std::string file);
		std::string	httpStatusToStr(HttpStatus code);
		std::string	getDefaultErrorPage(HttpStatus code);
	private :
		static const int				_method_num = 5;
		typedef int						(Query::*queryMethod)();
		static const queryMethod		_queryExecute[_method_num];
		std::string						_request_str;
		HttpStatus						_err_code;
		std::string						_resource;
		FileStatus						_resource_status;
		std::string						_header;
		unsigned long					_content_len;
		ContentTypes					_content_type;
		std::string						_http_redirect;

		Config							*_config;
		Client							*_client;
		Request							*_query;
		std::map<std::string, Cookie *>	*_all_cookies;
		std::vector<Cookie *>			_query_cookies;
};