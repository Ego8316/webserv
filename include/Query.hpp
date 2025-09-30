/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/30 14:20:57 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "utils.hpp"
#include "Config.hpp"

class Client;
class Request;
class Config;

class Query {
	public :
	// CONSTRUCTORS
		Query();
		Query(const Query &other);
		Query &operator=(const Query &other);
	//DESTUCTORS
		~Query();
	//GETTERS
	//SETTERS
	//MEMBER FUNCTIONS
		int			queryRespond(Client *client, Config *config);
		int			queryGet();
		int			queryPost();
		int			queryDelete();
		int			queryCGIRun();
		int			queryError();
		
		int			readRequest();
		int			setRessourceStatus();
		std::string	getRessourceTypeStr();
		void		setHeader();
		int			sendHeader();
		int			streamFile(std::string file);
	private :
		static const int			_method_num = 5;
		typedef int					(Query::*queryMethod)();
		static const queryMethod	_queryExecute[_method_num];
		std::string					_query_str;
		Request						*_query;
		int							_err_code;
		std::string					_ressource;
		int							_ressource_status;
		std::string					_header;
		unsigned long				_content_len;
		ContentTypes				_content_type;	//TODO implement setting this as part of setRessourceStatus
		Client						*_client;
		Config						*_config;
};
