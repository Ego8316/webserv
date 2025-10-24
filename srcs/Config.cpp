/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 16:34:44 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/24 17:11:21 by ego              ###   ########.fr       */
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
	{"enable_listdir", FieldHandler::BOOL, false, false, 0, 0,
		{ .bool_field = &Config::enable_listdir },
		{ .bool_value = DEFAULT_ENABLE_LISTDIR },
		NULL },
	{"default_page", FieldHandler::STRING, false, false, 0, 0,
		{ .string_field = &Config::default_page },
		{ .string_value = DEFAULT_DEFAULT_PAGE },
		NULL },
	{"default_error_pages", FieldHandler::LIST, false, false, 0, 0,
		{ 0 }, { 0 }, NULL },
	{"methods", FieldHandler::LIST, true, false, 0, 0,
		{ 0 }, { 0 }, NULL },
	{"http_redir", FieldHandler::LIST, false, false, 0, 0,
		{ 0 }, { 0 }, NULL },
};


Config::Config(const std::string &conf, const std::string &name)
{
	std::istringstream	conf_stream(conf);
	std::string			line;
	std::string			field, equal, value;

	this->_initEnumMaps();
	if (name.empty())
		this->server_name = utils::toString(_nb);
	else
		this->server_name = name;
	_nb++;
	while (std::getline(conf_stream, line))
	{
		++line_number;
		line = utils::stringTrim(line, " \t\n");
		if (line.empty()) continue ;
		std::istringstream	line_stream(line);
		if (!(line_stream >> field >> equal >> value) || (equal != "=" && equal != ":"))
			throw Error("Invalid line format");
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

Config::~Config() {}

const std::map<std::string, Redirection>	&Config::getRedirections() const
{
	return (this->http_redir);
}

bool	Config::isAcceptedMethod(Method element) const
{
	for (unsigned int i = 0; i < this->accepted_methods.size(); ++i)
		if (element == this->accepted_methods[i])
			return (true);
	return (false);
}

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
			else if (fh.name == "methods")
				_parseMethods(conf_stream);
			else
				_parseHttpRedir(conf_stream);
		}
	}
	return ;
}

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

void	Config::_parseDefaultErrorPages(std::istringstream &conf_stream)
{
	std::string	newline;
	int			field;
	std::string	equal, value;

	while (std::getline(conf_stream, newline))
	{
		++line_number;
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t\n") == "end")
			return ;
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

void	Config::_parseMethods(std::istringstream &conf_stream)
{
	std::string	newline;
	std::string	field;

	while (std::getline(conf_stream, newline))
	{
		++line_number;
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t\n") == "end")
			return ;
		std::istringstream 	line(newline);
		
		if (!(line >> field))
			throw Error("Invalid line format");
		if (utils::strToMethod(field) == UNKNOWN)
			throw Error("Unknown method: " + field);
		this->accepted_methods.push_back(utils::strToMethod(field));
	}
}

void	Config::_parseHttpRedir(std::istringstream &conf_stream)
{
	std::string	newline;
	std::string	path, equal, dest;
	int			error_code;

	while (std::getline(conf_stream, newline))
	{
		if (utils::stringTrim(newline, " \t\n").length() == 0)
			continue;
		else if (utils::stringTrim(newline, " \t\n") == "end")
			return ;
		std::istringstream	line(newline);
		
		if (!(line >> path >> equal >> dest >> error_code) || (equal != "=" && equal != ":"))
			throw Error("Invalid line format");
		this->http_redir[path].dest = dest;
		if ((300 <= error_code && error_code <= 302) || error_code == 308)
			this->http_redir[path].error_code = static_cast<HttpStatus>(error_code);
		else
			this->http_redir[path].error_code = HTTP_REDIRECT;
		if (newline.find("}") != std::string::npos)
			return ;
	}
}

void	Config::_assignDefault(FieldHandler &fh)
{
	if (fh.name == "default_error_pages" || fh.name == "http_redir")
		return ;
	switch (fh.type)
	{
		case FieldHandler::INT:		this->*(fh.target.int_field) = fh.default_value.int_value; break;
		case FieldHandler::LONG:	this->*(fh.target.long_field) = fh.default_value.long_value; break;
		case FieldHandler::SIZE:	this->*(fh.target.size_field) = fh.default_value.size_value; break;
		case FieldHandler::BOOL:	this->*(fh.target.bool_field) = fh.default_value.bool_value; break;
		case FieldHandler::STRING:	this->*(fh.target.string_field) = fh.default_value.string_value; break;
		default:					break;
	}
}

std::ostream &operator<<(std::ostream &os, const Config &cfg)
{
	#define BORDER_COLOR		BOLD_PURPLE
	#define SECTION_COLOR		BOLD_RED
	#define FIELD_NAME_COLOR	BOLD
	#define	FIELD_VALUE_COLOR	RESET
	#define WIDTH				70UL
	#define SECTION(title) \
		{ \
			os << BORDER_COLOR << VERTICAL << RESET << " "; \
			os << SECTION_COLOR << title << RESET; \
			int pad = WIDTH - strlen(title) - 2; \
			for (int i = 0; i <= pad; ++i) os << " "; \
			os << BORDER_COLOR <<  VERTICAL << RESET << "\n"; \
		}
	#define FIELD(name, value) \
		{ \
			os << BORDER_COLOR << VERTICAL << RESET << "   "; \
			if (strlen(name) < WIDTH / 2) \
			{ \
				os << FIELD_NAME_COLOR << name << RESET;\
				for (size_t i = 0; i < WIDTH / 2 - strlen(name); ++i) os << " "; \
			} \
			else \
				for (size_t i = 0; i < WIDTH / 2; ++i) os << " "; \
			if (strlen(value) < WIDTH / 2 - 3) \
			{ \
				os << FIELD_VALUE_COLOR << value << RESET; \
				for (size_t i = 0; i < WIDTH / 2 - 3 - strlen(value); ++i) os << " "; \
			} \
			else \
				for (size_t i = 0; i < WIDTH / 2 - 3; ++i) os << " "; \
			os << BOLD_PURPLE << VERTICAL << RESET << "\n"; \
		}

	os << BORDER_COLOR << TOP_LEFT << "SERVER CONFIG";
	for (size_t i = 0; i < WIDTH - 13; ++i) os << HORIZONTAL;
	os << TOP_RIGHT << RESET << "\n";
	SECTION("Server settings");
	FIELD("Name:", cfg.server_name.c_str());
	FIELD("IP:", utils::toString(cfg.ip).c_str());
	FIELD("Port:", utils::toString(cfg.port_number).c_str());
	FIELD("Domain:", utils::toString(cfg.domain).c_str());
	FIELD("Type:", utils::toString(cfg.type).c_str());
	FIELD("Protocol:", utils::toString(cfg.protocol).c_str());
	SECTION("Server limits");
	FIELD("Max header size:", utils::toString(cfg.max_header_size).c_str());
	FIELD("Max body size:", utils::toString(cfg.max_body_size).c_str());
	FIELD("Client limit:", utils::toString(cfg.client_limit).c_str());
	FIELD("Processing time:", utils::toString(cfg.processing_time_limit).c_str());
	FIELD("Max request time:", utils::toString(cfg.max_request_time).c_str());
	FIELD("Incoming queue:", utils::toString(cfg.incoming_queue_backlog).c_str());
	FIELD("Buffer size:", utils::toString(cfg.buffer_size).c_str());
	FIELD("Cookie session:", utils::toString(cfg.cookie_sessions_max).c_str());
	FIELD("Cookie lifetime:", utils::toString(cfg.cookie_life_time).c_str());
	SECTION("Files & Directories");
	FIELD("Root:", utils::toString(cfg.server_home.c_str()).c_str());
	FIELD("Listdir:", (const char *)(cfg.enable_listdir ? "ON" : "OFF"));
	FIELD("Default page:", utils::toString(cfg.default_page.c_str()).c_str());
	SECTION("Methods");
	for (size_t j = 0; j < cfg.accepted_methods.size(); ++j)
		FIELD(utils::methodToStr(cfg.accepted_methods[j]).c_str(), "");
	SECTION("Default error pages");
	for (std::map<int, std::string>::const_iterator it = cfg.default_error_pages.begin(); it != cfg.default_error_pages.end(); ++it)
		FIELD(utils::toString(it->first).c_str(), (it->second).c_str());
	SECTION("Redirections");
	for (std::map<std::string, Redirection>::const_iterator it = cfg.http_redir.begin(); it != cfg.http_redir.end(); ++it)
		FIELD(it->first.c_str(), (it->second.dest + " (" + utils::toString(it->second.error_code) + ")").c_str());
	os << BOLD_PURPLE << BOTTOM_LEFT;
	for (size_t i = 0; i < WIDTH; ++i) os << HORIZONTAL;
	os << BOTTOM_RIGHT << RESET << "\n";
	#undef FIELD
	#undef SECTION
	#undef WIDTH
	#undef FIELD_VALUE_COLOR
	#undef FIELD_NAME_COLOR
	#undef SECTION_COLOR
	#undef BORDER_COLOR
	return (os);
}
