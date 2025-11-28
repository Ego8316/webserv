/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:36:39 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/28 11:51:51 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

class	Config
{
	public:
		Config(const std::string &conf, const std::string &name);
		~Config();

		class	Error	:	public	std::exception
		{
			std::string	_msg;
			public:
				Error(const std::string &msg) { _msg = msg; }
				virtual ~Error() throw() {};
				const char* what() const throw() { return _msg.c_str(); }
		};

		typedef struct	s_Location
		{
			std::string							root;
			std::string							default_page;
			Method								accepted_methods;
			bool								autoindex;
			std::map<std::string, Redirection>	redirs;
			bool								has_default_page;
			bool								has_methods;
			bool								has_autoindex;
		}	Location;

		bool										isAcceptedMethod(Method method, const Location &loc) const;

		static int							line_number;
		std::string							server_name;
		unsigned int						ip;
		int									port_number;
		int									domain;
		int									type;
		int									protocol;
		size_t								max_header_size;
		size_t								max_body_size;
		int									client_limit;
		long								processing_time_limit;
		long								max_request_time;
		int									incoming_queue_backlog;
		size_t								buffer_size;
		int									cookie_sessions_max;
		int									cookie_life_time;
		std::string							server_home;
		std::map<std::string, Location>		locations;
		std::string							default_page;
		bool								default_autoindex;
		Method								default_accepted_methods;
		std::map<int, std::string>			default_error_pages;

	private:
		Config(const Config &other);
		Config	&operator=(const Config &other);

		typedef struct	s_FieldHandler
		{
			std::string	name;
			enum FieldType
			{
				UINT,
				INT,
				LONG,
				SIZE,
				BOOL,
				STRING,
				ENUM,
				LIST
			}	type;
			bool	required;
			bool	found;
			size_t	min;
			size_t	max;
			union
			{
				unsigned int	Config::*uint_field;
				int				Config::*int_field;
				Method			Config::*method_field;
				long			Config::*long_field;
				size_t			Config::*size_field;
				bool			Config::*bool_field;
				std::string		Config::*string_field;
			}	target;
			union
			{
				int			int_value;
				long		long_value;
				size_t		size_value;
				bool		bool_value;
				const char	*string_value;
			}	default_value;
			std::map<std::string, int>	Config::*enum_map;
		}	FieldHandler;

		static int					_nb;
		static FieldHandler			_fields[];
		std::map<std::string, int>	_enum_domain;
		std::map<std::string, int>	_enum_type;
		std::map<std::string, int>	_enum_protocol;

		void	_initEnumMaps();
		void	_assignValue(FieldHandler &fh, const std::string &value, std::istringstream &conf_stream);
		void	_parseLocation(const std::string &path, const std::string &bracket, std::istringstream &conf_stream);
		void	_parseLocationMethods(Location &loc, std::istringstream &conf_stream);
		void	_parseLocationRedirs(Location &loc, std::istringstream &conf_stream);
		void	_parseIP(const std::string &ip_str);
		void	_parseDefaultErrorPages(std::istringstream &conf_stream);
		void	_parseDefaultMethods(std::istringstream &conf_stream);
		void	_assignDefault(FieldHandler &fh);
};

std::ostream	&operator<<(std::ostream &os, const Config &src);
