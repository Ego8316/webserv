/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:36:39 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/24 00:13:05 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

class	Config
{
	public:
		Config(const std::string &conf);
		~Config();

		class	Error	:	public	std::exception
		{
			std::string	_msg;
			public:
				Error(const std::string &msg) { _msg = msg; }
				const char* what() const throw() { return _msg.c_str(); }
		};

		const std::map<std::string, Redirection>	&getRedirections() const;
		bool										isAcceptedMethod(Method method) const;

		unsigned int							ip;
		int										port_number;
		int										domain;
		int										type;
		int										protocol;
		size_t									max_header_size;
		size_t									max_body_size;
		int										client_limit;
		long									processing_time_limit;
		long									max_request_time;
		int										incoming_queue_backlog;
		int										buffer_size;
		int										cookie_sessions_max;
		int										cookie_life_time;
		std::string								server_home;
		bool									enable_listdir;
		std::string								default_page;
		std::map<int, std::string>				default_error_pages;
		std::vector<Method>						accepted_methods;
		std::map<std::string, Redirection>		http_redir;

	private:
		Config(const Config &other);
		Config	&operator=(const Config &other);

		typedef struct s_FieldHandler
		{
			std::string	name;
			enum FieldType
			{
				INT,
				LONG,
				STRING,
				BOOL,
				ENUM
			}	type;
			bool	required;
			bool	found;
			size_t	min;
			size_t	max;
			union
			{
				unsigned int	Config::*uintField;
				int				Config::*intField;
				long			Config::*longField;
				size_t			Config::*sizeField;
				bool			Config::*boolField;
				std::string		Config::*stringField;
			}	target;
			union
			{
				int			intDefault;
				bool		boolDefault;
				size_t		sizeDefault;
				bool		boolDefault;
				std::string	stringDefault;
			}	default_value;
		}	FieldHandler;

		static FieldHandler	_fields[];
		static int			_line_number;

		void	_assignValue(FieldHandler &fh, const std::string &value);
		void	_setIP(std::string ip_str);
		void	_parseDefaultErrorPages(std::istringstream &conf_stream);
		void	_parseMethod(std::istringstream &conf_stream);
		void	_parseHttpRedir(std::istringstream &conf_stream);
};

std::ostream	&operator<<(std::ostream &os, const Config &src);
