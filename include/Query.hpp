/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:19:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/28 17:24:24 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "utils.hpp"

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

		int			queryRespond();
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
		Request						_query;
		int							_err_code;
		std::string					_ressource;
		int							_ressource_status;
		std::string					_header;
		unsigned long				_content_len;
		ContentTypes				_content_type;	//TODO implement setting this as part of setRessourceStatus
		Client						_client;
};

const Query::queryMethod	Query::_queryExecute[_method_num] = {
	&Query::queryGet,
	&Query::queryPost, 
	&Query::queryDelete,
	&Query::queryCGIRun,
	&Query::queryError
};
