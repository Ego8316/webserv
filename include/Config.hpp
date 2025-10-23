/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:36:39 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/23 22:24:38 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

class	Config
{
	public:
		Config(std::string filename);
		~Config();

		std::map<std::string, Redirection>		getRedirections() const;
		void			setIP(std::string ip_str);

		void			parseDefaultErrorPages(std::istringstream &conf_stream);
		void			parseMethod(std::istringstream &conf_stream);
		void			parseHttpRedir(std::istringstream &conf_stream);
		bool			isAcceptedMethod(Method method) const;

		unsigned int	ip;
		int				port_number;
		int				domain;
		int				type;
		int				protocol;
		size_t			max_header_size;
		size_t			max_body_size;
		int				client_limit;
		long			processing_time_limit;
		long			max_request_time;
		int				incoming_queue_backlog;
		int				buffer_size;
		int				cookie_sessions_max;
		int				cookie_life_time;
		std::string		server_home;
		bool			enable_listdir;
		std::string		default_page;
		
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
				size_t		Config::*intField;
				bool		Config::*boolField;
				std::string	Config::*stringField;
			}	target;
			
			void (Config::*stringParser)(const std::string&);
			void (Config::*streamParser)(std::istringstream &);
		}	FieldHandler;
};

std::ostream	&operator<<(std::ostream &os, const Config &src);
