/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/11/24 23:46:11 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

int	Config::line_number = 1;

int	Config::_nb = 1;

Config::FieldHandler Config::_fields[] = {
	{"ip", FieldHandler::UINT, true, false, 0, 0,
		{ .uint_field = &Config::ip },
		{ .int_value = INADDR_ANY },
		NULL },
	{"port", FieldHandler::INT, true, false, MIN_PORT, MAX_PORT,
		{ .int_field = &Config::port_number },
		{ .int_value = DEFAULT_PORT },
		NULL },
	{"domain", FieldHandler::ENUM, true, false, 0, 0,
		{ .int_field = &Config::domain },
		{ .int_value = AF_INET },
		&Config::_enum_domain },
	{"type", FieldHandler::ENUM, true, false, 0, 0,
		{ .int_field = &Config::type },
		{ .int_value = SOCK_STREAM },
		&Config::_enum_type },
	{"protocol", FieldHandler::ENUM, true, false, 0, 0,
		{ .int_field = &Config::protocol },
		{ .int_value = 0 },
		&Config::_enum_protocol },
	{"max_header_size", FieldHandler::SIZE, false, false, MIN_MAX_HEADER_SIZE, MAX_MAX_HEADER_SIZE,
		{ .size_field = &Config::max_header_size },
		{ .size_value = DEFAULT_MAX_HEADER_SIZE },
		NULL },
	{"max_body_size", FieldHandler::SIZE, false, false, MIN_MAX_BODY_SIZE, MAX_MAX_BODY_SIZE,
		{ .size_field = &Config::max_body_size },
		{ .size_value = DEFAULT_MAX_BODY_SIZE },
		NULL },
	{"client_limit", FieldHandler::INT, false, false, MIN_CLIENT_LIMIT, MAX_CLIENT_LIMIT,
		{ .int_field = &Config::client_limit },
		{ .int_value = DEFAULT_CLIENT_LIMIT },
		NULL },
	{"processing_time_limit", FieldHandler::LONG, false, false, MIN_PROCESSING_TIME_LIMIT, MAX_PROCESSING_TIME_LIMIT,
		{ .long_field = &Config::processing_time_limit },
		{ .long_value = DEFAULT_PROCESSING_TIME_LIMIT },
		NULL },
	{"max_request_time", FieldHandler::LONG, false, false, MIN_MAX_REQUEST_TIME, MAX_MAX_REQUEST_TIME,
		{ .long_field = &Config::max_request_time },
		{ .long_value = DEFAULT_MAX_REQUEST_TIME },
		NULL },
	{"incoming_queue_backlog", FieldHandler::INT, false, false, MIN_INCOMING_QUEUE_BACKLOG, MAX_INCOMING_QUEUE_BACKLOG,
		{ .int_field = &Config::incoming_queue_backlog },
		{ .int_value = DEFAULT_INCOMING_QUEUE_BACKLOG },
		NULL },
	{"buffer_size", FieldHandler::SIZE, false, false, MIN_BUFFER_SIZE, MAX_BUFFER_SIZE,
		{ .size_field = &Config::buffer_size },
		{ .size_value = DEFAULT_BUFFER_SIZE },
		NULL },
	{"cookie_sessions_max", FieldHandler::INT, false, false, MIN_COOKIE_SESSIONS_MAX, MAX_COOKIE_SESSIONS_MAX,
		{ .int_field = &Config::cookie_sessions_max },
		{ .int_value = DEFAULT_COOKIE_SESSIONS_MAX },
		NULL },
	{"cookie_life_time", FieldHandler::INT, false, false, MIN_COOKIE_LIFETIME, MAX_COOKIE_LIFETIME,
		{ .int_field = &Config::cookie_life_time },
		{ .int_value = DEFAULT_MAX_COOKIE_LIFETIME },
		NULL },
	{"server_home", FieldHandler::STRING, true, false, 0, 0,
		{ .string_field = &Config::server_home },
		{ .string_value = DEFAULT_SERVER_HOME },
		NULL },
	{"autoindex", FieldHandler::BOOL, false, false, 0, 0,
		{ .bool_field = &Config::default_autoindex },
		{ .bool_value = DEFAULT_AUTOINDEX },
	NULL },
	{"default_page", FieldHandler::STRING, false, false, 0, 0,
		{ .string_field = &Config::default_page },
		{ .string_value = DEFAULT_DEFAULT_PAGE },
		NULL },
	{"methods", FieldHandler::LIST, false, false, 0, 0,
		{ .method_field = &Config::default_accepted_methods },
		{ .int_value = DEFAULT_ACCEPTED_METHODS },
		NULL },
	{"default_error_pages", FieldHandler::LIST, false, false, 0, 0,
		{ 0 }, { 0 }, NULL },
};

/**
 * @brief Parses a server block string into a Config instance.
 *
 * @param conf Raw server block content.
 * @param name Server name extracted from declaration.
 */
Config::Config(const std::string &conf, const std::string &name)
{
	std::istringstream	conf_stream(conf);
	std::string			line, field, equal, value;
	bool				parsing_locations = false;

	this->_initEnumMaps();
	for (size_t i = 0; i < sizeof(_fields) / sizeof(_fields[0]); ++i) _fields[i].found = false;
	if (name.empty())
		this->server_name = utils::toString(_nb);
	else
		this->server_name = name;
	this->_nb++;
	this->default_accepted_methods = UNKNOWN;
	while (std::getline(conf_stream, line))
	{
		++line_number;
		if (line.empty() || line[0] == '#') continue ;
		std::istringstream	line_stream(line);
		if (!(line_stream >> field >> equal >> value) || !line_stream.eof())
			throw Error("Invalid line format");
		if (field == "location")
		{
			parsing_locations = true;
			_parseLocation(equal, value, conf_stream);
			continue ;
		}
		if (parsing_locations)
			throw Error("Unexpected content after a location block");
		if (equal != "=" && equal != ":")
			throw Error("Unexpected token in second position: expected `=' or `:'");
		bool	matched = false;
		for (size_t i = 0; i < sizeof(_fields) / sizeof(_fields[0]); ++i)
		{
			FieldHandler	&fh = this->_fields[i];
			if (fh.name == utils::toLower(field))
			{
				if (fh.found == true)
					throw Error("Duplicate");
				fh.found = true;
				this->_assignValue(fh, utils::toLower(value), conf_stream);
				matched = true;
				break ;
			}
		}
		if (!matched)
			throw Error("Unknown config field: " + field);
	}
	for (size_t i = 0; i < sizeof(_fields) / sizeof(_fields[0]); ++i)
	{
		FieldHandler	&fh = _fields[i];
		if (!fh.found && fh.required)
			throw Error("Missing config field: " + fh.name);
		if (!fh.found)
			this->_assignDefault(fh);
	}
}

/**
 * @brief Destructor logging config teardown.
 */
Config::~Config()
{
	std::cerr << RED << "Destroying config" << RESET << std::endl;
}

/**
 * @brief Checks if a method is allowed for a given location.
 *
 * @param element Method to test.
 * @param loc Target location.
 *
 * @return True when allowed.
 */
bool	Config::isAcceptedMethod(Method element, const Location &loc) const
{
	return (loc.accepted_methods & element);
}

/**
 * @brief Fills enum maps for domain/type/protocol keywords.
 */
void	Config::_initEnumMaps()
{
	for (size_t i = 0; i < sizeof(_fields) / sizeof(_fields[0]); ++i)
	{
		if (_fields[i].enum_map == NULL)
			continue ;
		std::map<std::string, int> &emap = this->*(_fields[i].enum_map);
		if (_fields[i].name == "domain")
		{
			emap["af_inet"] = AF_INET;
			emap["af_unix"] = AF_UNIX;
			emap["inet"] = AF_INET;
			emap["unix"] = AF_UNIX;
		}
		else if (_fields[i].name == "type")
		{
			emap["sock_stream"] = SOCK_STREAM;
			emap["sock_dgram"] = SOCK_DGRAM;
			emap["stream"] = SOCK_STREAM;
			emap["dgram"] = SOCK_DGRAM;
		}
		else if (_fields[i].name == "protocol")
		{
			emap["ipproto_tcp"] = IPPROTO_TCP;
			emap["ipproto_udp"] = IPPROTO_UDP;
			emap["tcp"] = IPPROTO_TCP;
			emap["udp"] = IPPROTO_UDP;
			for (int j = 0; j < 10; ++j)
				emap[utils::toString(j)] = j;
		}
	}
}

/**
 * @brief Parses a location block and stores it in the map.
 *
 * @param path Location path.
 * @param bracket Opening bracket token.
 * @param conf_stream Stream positioned after declaration.
 */
void	Config::_parseLocation(const std::string &path, const std::string &bracket, std::istringstream &conf_stream)
{
	if (bracket != "{")
		throw Error("Expected `{' after location path");
	Location	loc;
	loc.autoindex = this->default_autoindex; 
	loc.root = this->server_home;
	loc.default_page = this->default_page;
	loc.accepted_methods = this->default_accepted_methods;

	std::string newline;
	while (std::getline(conf_stream, newline))
	{
		++line_number;
		newline = utils::stringTrimSpaces(newline);
		if (newline.empty()) continue;
		if (newline == "}") break;
		std::istringstream line(newline);
		std::string			field, equal, value;
		if (!(line >> field >> equal >> value) )
			throw Error("Invalid location line format");
		if (equal != "=" && equal != ":")
			throw Error("Unexpected token in location block (expected `=' or `:')");
		if (field == "root")
			loc.root = value;
		else if (field == "autoindex")
			loc.autoindex = (value == "true" || value == "on");
		else if (field == "default_page")
			loc.default_page = value;
		else if (field == "methods")
			_parseLocationMethods(loc, conf_stream);
		else if (field == "http_redir")
			_parseLocationRedirs(loc, conf_stream);
		else
			throw Error("Unknown directive in location block: " + field);
	}
	this->locations[path] = loc;
}

/**
 * @brief Parses the METHODS list inside a location block.
 *
 * @param loc Location to populate.
 * @param conf_stream Stream positioned after METHODS keyword.
 */
void	Config::_parseLocationMethods(Location &loc, std::istringstream &conf_stream)
{
	std::string	newline;
	std::string	field;
	Method		method;

	loc.accepted_methods = UNKNOWN;
	while (std::getline(conf_stream, newline))
	{
		++line_number;
		newline = utils::toLower(utils::stringTrimSpaces(newline));
		if (newline.length() == 0) continue ;
		else if (newline == "end") break ;
		std::istringstream 	line(newline);
		
		if (!(line >> field))
			throw Error("Invalid line format");
		method = utils::strToMethod(utils::toUpper(field));
		if (method == UNKNOWN)
			throw Error("Unknown method: " + field);
		loc.accepted_methods |= method;
	}
}


/**
 * @brief Parses HTTP redirection entries inside a location block.
 *
 * @param loc Location to populate.
 * @param conf_stream Stream positioned after http_redir keyword.
 */
void	Config::_parseLocationRedirs(Location &loc, std::istringstream &conf_stream)
{
	std::string	line;
	while (std::getline(conf_stream, line))
	{
		++line_number;
		line = utils::stringTrimSpaces(line);
		if (line.empty()) continue;
		if (line == "end") return;

		std::istringstream iss(line);
		std::string path, equal, dest;
		int code;
		if (!(iss >> path >> equal >> dest >> code))
			throw Error("Invalid redirect format in location");

		if (equal != "=" && equal != ":")
			throw Error("Expected `=' or `:' in redirection line");

		Redirection	r;
		r.dest = dest;
		// TODO meilleur check de la validite de l'error code?
		r.error_code = (300 <= code && code <= 308) ? static_cast<HttpStatus>(code) : HTTP_REDIRECT;
		loc.redirs[path] = r;
	}
}

/**
 * @brief Assigns a directive value to the appropriate field.
 *
 * @param fh Field handler metadata.
 * @param value Parsed value string.
 * @param conf_stream Stream used for list directives.
 */
void	Config::_assignValue(FieldHandler &fh, const std::string &value, std::istringstream &conf_stream)
{
	switch (fh.type)
	{
		case FieldHandler::UINT:
		{
			_parseIP(value);
			break ;
		}
		case FieldHandler::INT:
		{
			int	v = atoi(value.c_str());
			if (v < static_cast<int>(fh.min) || v > static_cast<int>(fh.max))
				throw Error("Value out of range for " + fh.name);
			this->*(fh.target.int_field) = v;
			break ;
		}
		case FieldHandler::LONG:
		{
			long v = strtol(value.c_str(), 0 ,0);
			if (v < static_cast<long>(fh.min) || v > static_cast<long>(fh.max))
				throw Error("Value out of range for " + fh.name);
			this->*(fh.target.long_field) = v;
			break ;
		}
		case FieldHandler::SIZE:
		{
			size_t v = strtoul(value.c_str(), 0 ,0);
			if (v < fh.min || v > fh.max)
				throw Error("Value out of range for " + fh.name);
			this->*(fh.target.size_field) = v;
			break ;
		}
		case FieldHandler::BOOL:
		{
			if (value != "true" && value != "false")
				throw Error("Unknown keyword for " + fh.name);
			this->*(fh.target.bool_field) = (value == "true");
			break ;
		}
		case FieldHandler::STRING:
		{
			this->*(fh.target.string_field) = value;
			break ;
		}
		case FieldHandler::ENUM:
		{
			std::map<std::string, int>				&emap = this->*(fh.enum_map);
			std::map<std::string, int>::iterator	it = emap.find(value);
			if (it == emap.end())
				throw Error("Unknown keyword for " + fh.name);
			this->*(fh.target.int_field) = it->second;
			break ;
		}
		case FieldHandler::LIST:
		{
			if (fh.name == "default_error_pages")
				_parseDefaultErrorPages(conf_stream);
			else
				_parseDefaultMethods(conf_stream);
		}
	}
	return ;
}

/**
 * @brief Parses dotted IPv4 strings or INADDR_* aliases.
 *
 * @param ip_str IP string.
 */
void	Config::_parseIP(const std::string &ip_str)
{
	int					octets[4];
	char				dot1, dot2, dot3;
	std::istringstream	iss(ip_str);

	if (ip_str == "INADDR_ANY" || ip_str == "localhost")
	{
		this->ip = htonl(INADDR_ANY);
		return ;
	}
	else if (ip_str == "INADDR_NONE")
	{
		this->ip = htonl(INADDR_NONE);
		return ;
	}
	if (!(iss >> octets[0] >> dot1 >> octets[1] >> dot2 >> octets[2] >> dot3 >> octets[3]) ||
		dot1 != '.' || dot2 != '.' || dot3 != '.' || !iss.eof())
		throw Error("Invalid IP format: " + ip_str);
	for (int i = 0; i < 4; ++i)
	{
		if (octets[i] < 0 || octets[i] > 255)
			throw Error("Invalid IP address (out of range): " + ip_str);
	}
	this->ip = (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
	this->ip = htonl(this->ip);
}

/**
 * @brief Parses a DEFAULT_ERROR_PAGES dictionary block.
 *
 * @param conf_stream Stream positioned after directive.
 */
void	Config::_parseDefaultErrorPages(std::istringstream &conf_stream)
{
	std::string	newline;
	int			field;
	std::string	equal, value;

	while (std::getline(conf_stream, newline))
	{
		++line_number;
		newline = utils::stringTrimSpaces(newline);
		if (newline.length() == 0) continue;
		else if (newline == "end") return ;
		std::istringstream 	line(newline);
		if (!(line >> field >> equal >> value) || (equal != "=" && equal != ":"))
			throw Error("Invalid line format");
		// if (utils::httpStatusToStr(field) == "Unknown")
		if (field < 200 || field > 600)
			throw Error("Unknown or unsupported error number in default error pages definition");
		// TODO mieux checker
		this->default_error_pages[field] = value;
	}
}

/**
 * @brief Parses a METHODS list block at server level.
 *
 * @param conf_stream Stream positioned after METHODS directive.
 */
void	Config::_parseDefaultMethods(std::istringstream &conf_stream)
{
	std::string	newline;
	std::string	field;
	Method		method;

	while (std::getline(conf_stream, newline))
	{
		++line_number;
		newline = utils::toLower(utils::stringTrimSpaces(newline));
		if (newline.length() == 0) continue ;
		else if (newline == "end") break ;
		std::istringstream 	line(newline);
		
		if (!(line >> field))
			throw Error("Invalid line format");
		method = utils::strToMethod(utils::toUpper(field));
		if (method == UNKNOWN)
			throw Error("Unknown method: " + field);
		this->default_accepted_methods |= method;
	}
}

/**
 * @brief Assigns default values when an optional directive is absent.
 *
 * @param fh Field handler metadata.
 */
void	Config::_assignDefault(FieldHandler &fh)
{
	if (fh.name == "default_error_pages")
		return ;
	switch (fh.type)
	{
		case FieldHandler::INT:		this->*(fh.target.int_field) = fh.default_value.int_value; break;
		case FieldHandler::LONG:	this->*(fh.target.long_field) = fh.default_value.long_value; break;
		case FieldHandler::SIZE:	this->*(fh.target.size_field) = fh.default_value.size_value; break;
		case FieldHandler::BOOL:	this->*(fh.target.bool_field) = fh.default_value.bool_value; break;
		case FieldHandler::STRING:	this->*(fh.target.string_field) = fh.default_value.string_value; break;
		case FieldHandler::LIST:	this->*(fh.target.method_field) = static_cast<Method>(fh.default_value.int_value); break;
		default:					break;
	}
}

namespace
{
	#define WIDTH				70UL
	#define BORDER_COLOR		BOLD_PURPLE
	#define SECTION_COLOR		BOLD_RED
	#define SECTION_SUB_COLOR	BLUE
	#define FIELD_NAME_COLOR	BOLD
	#define	FIELD_VALUE_COLOR	RESET

	/**
	 * @brief Prints the top border with a title.
	 * 
	 * @param os Output stream.
	 * @param title Title to display.
	 */
	void	printBorderTop(std::ostream &os, const std::string &title)
	{
		os << BORDER_COLOR << TOP_LEFT << title;
		for (size_t i = 0; i < WIDTH - title.size(); ++i) { os << HORIZONTAL; }
		os << TOP_RIGHT << RESET << "\n";
	}

	/**
	 * @brief Prints the bottom border.
	 * 
	 * @param os Output stream.
	 */
	void	printBorderBottom(std::ostream &os)
	{
		os << BORDER_COLOR << BOTTOM_LEFT;
		for (size_t i = 0; i < WIDTH; ++i) { os << HORIZONTAL; }
		os << BOTTOM_RIGHT << RESET << "\n";
	}

	/**
	 * @brief Prints a titled section with optional subtitle.
	 *
	 * @param os Output stream.
	 * @param title Section title.
	 * @param subtitle Section subtitle.
	 */
	void	printSection(std::ostream &os, const std::string &title, const std::string &subtitle)
	{
		os << BORDER_COLOR << VERTICAL << RESET << " ";
		os << SECTION_COLOR << title << RESET;
		for (size_t i = 0; i <= WIDTH / 2 - static_cast<int>(title.length()); ++i) { os << " "; }
		if (subtitle.length() < WIDTH / 2 - 3)
		{
			os << SECTION_SUB_COLOR << subtitle << RESET;
			for (size_t i = 0; i < WIDTH / 2 - 2 - subtitle.length(); ++i) { os << " "; }
		}
		else
			for (size_t i = 0; i < WIDTH / 2 - 2; ++i) { os << " "; }
		os << BORDER_COLOR << VERTICAL << RESET << "\n";
	}

	/**
	 * @brief Prints a name/value row inside the bordered box.
	 * 
	 * @param os Output stream.
	 * @param name Field name.
	 * @param value Field value.
	 */
	void	printField(std::ostream &os, const std::string &name, const std::string &value)
	{
		os << BORDER_COLOR << VERTICAL << RESET << "   ";
		if (name.length() < WIDTH / 2)
		{
			os << FIELD_NAME_COLOR << name << RESET;
			for (size_t i = 0; i < WIDTH / 2 - name.length(); ++i) { os << " "; }
		}
		else
			for (size_t i = 0; i < WIDTH / 2; ++i) { os << " " ; }
		if (value.length() < WIDTH / 2 - 3)
		{
			os << FIELD_VALUE_COLOR << value << RESET;
			for (size_t i = 0; i < WIDTH / 2 - 3 - value.length(); ++i) { os << " "; }
		}
		else
			for (size_t i = 0; i < WIDTH / 2 - 3; ++i) { os << " "; }
		os << BORDER_COLOR << VERTICAL << RESET << "\n";
	}

	/**
	 * @brief Prints general server settings.
	 * 
	 * @param os Output stream.
	 * @param cfg Configuration object.
	 */
	void	printServerSettings(std::ostream &os, const Config &cfg)
	{
		printSection(os, "Server settings", "");
		printField(os, "Name:", cfg.server_name);
		printField(os, "IP:", utils::toString(cfg.ip));
		printField(os, "Port:", utils::toString(cfg.port_number));
		printField(os, "Domain:", utils::toString(cfg.domain));
		printField(os, "Type:", utils::toString(cfg.type));
		printField(os, "Protocol:", utils::toString(cfg.protocol));
	}

	/**
	 * @brief Prints server limit settings.
	 * 
	 * @param os Output stream.
	 * @param cfg Configuration object.
	 */
	void	printServerLimits(std::ostream &os, const Config &cfg)
	{
		printSection(os, "Server limits", "");
		printField(os, "Max header size:", utils::toString(cfg.max_header_size));
		printField(os, "Max body size:", utils::toString(cfg.max_body_size));
		printField(os, "Client limit:", utils::toString(cfg.client_limit));
		printField(os, "Processing time:", utils::toString(cfg.processing_time_limit));
		printField(os, "Max request time:", utils::toString(cfg.max_request_time));
		printField(os, "Incoming queue:", utils::toString(cfg.incoming_queue_backlog));
		printField(os, "Buffer size:", utils::toString(cfg.buffer_size));
		printField(os, "Cookie session:", utils::toString(cfg.cookie_sessions_max));
		printField(os, "Cookie lifetime:", utils::toString(cfg.cookie_life_time));
	}

	void	printLocation(std::ostream &os, const Config::Location &loc, const std::string &route)
	{
		printSection(os, "Location", route);
		printField(os, "Root:", loc.root);
		printField(os, "Default page:", loc.default_page);
		printField(os, "Autoindex:", loc.autoindex ? "ON" : "OFF");
		printField(os, "Methods:", "");
		for (int i = 0; i < 3; ++i)
			if (loc.accepted_methods & (1 << i))
				printField(os, "   " + utils::methodToStr(static_cast<Method>(1 << i)), "");
		printField(os, "Redirections:", "");
		for (std::map<std::string, Redirection>::const_iterator it = loc.redirs.begin(); it != loc.redirs.end(); ++it)
			printField(os, it->first.c_str(), (it->second.dest + " (" + utils::toString(it->second.error_code) + ")").c_str());
	}
}

std::ostream &operator<<(std::ostream &os, const Config &cfg)
{
	printBorderTop(os, "SERVER CONFIG");
	printServerSettings(os, cfg);
	printServerLimits(os, cfg);
	for (std::map<std::string, Config::Location>::const_iterator it = cfg.locations.begin(); it != cfg.locations.end(); ++it)
		printLocation(os, it->second, it->first);
	printBorderBottom(os);
	return os;
}
