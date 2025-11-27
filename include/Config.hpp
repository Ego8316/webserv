/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:36:39 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/27 03:48:44 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "headers.hpp"
#include "utils.hpp"

/**
 * @class Config
 *
 * @brief Parses and stores server configuration including locations and limits.
 */
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
			std::map<std::string, s_Redirection>	redirs;
			bool								has_default_page;
			bool								has_methods;
			bool								has_autoindex;
		}	Location;

		bool										isAcceptedMethod(Method method, const Location &loc) const;

		static int							line_number;				// Removed
		std::string							server_name;				// Unchanged
		unsigned int						ip;							// Changed to listen_host
		int									port_number;				// Changed to listen_port
		int									domain;						// If only IPv4, switch to hard-coded default
		int									type;						// Changed to hard-coded default
		int									protocol;					// Changed to hard-coded default
		size_t								max_header_size;			// Hard-coded default?
		size_t								max_body_size;				// Changed to client_max_body_size
		int									client_limit;				// Hard-coded default?
		long								processing_time_limit;		// Hard-coded default?
		long								max_request_time;			// Hard-coded default?
		int									incoming_queue_backlog;		// Hard-coded default?
		size_t								buffer_size;				// Changed to client_header_buffer_size  and client_body_buffer_size
		int									cookie_sessions_max;		// Removed
		int									cookie_life_time;			// Removed
		std::string							server_home;				// Changed to root
		std::map<std::string, Location>		locations;					// Unchanged
		std::string							default_page;				// Changed to index
		bool								default_autoindex;			// Changed to autoindex
		Method								default_accepted_methods;	// Changed to methods
		std::map<int, std::string>			default_error_pages;		// Changed to error_pages

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
