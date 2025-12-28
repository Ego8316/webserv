/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:15:31 by ego               #+#    #+#             */
/*   Updated: 2025/12/29 00:01:24 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <signal.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <strings.h>
#include <algorithm>
#include <deque>
#include <dirent.h>
#include <vector>
#include <map>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <ctime>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "colors.hpp"

#if defined(__APPLE__)
	#define OS_NAME "macOS"
#elif defined(__linux__)
	#define OS_NAME "Linux"
#else
	#define OS_NAME "Unknown"
#endif

#define SERV_ERROR	-1
#define	WBLOCK		-2
#define NEW_CLIENT	1

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

// Default pages
#define POST_PAGE		"<html><body><h1>Upload successful</h1></body></html>"

#define ERROR_PAGE_400	"<html><head><title>400 Bad Request</title></head>" \
						"<body><h1>400 Bad Request</h1>" \
						"<p>Your browser sent a request that this server could not understand.</p></body></html>"

#define ERROR_PAGE_403	"<html><head><title>403 Forbidden</title></head>" \
						"<body><h1>403 Forbidden</h1>" \
						"<p>You don't have permission to access this resource.</p></body></html>"

#define ERROR_PAGE_404	"<html><head><title>404 Not Found</title></head>" \
						"<body><h1>404 Not Found</h1>" \
						"<p>The requested URL was not found on this server.</p></body></html>"

#define ERROR_PAGE_405	"<html><head><title>405 Method Not Allowed</title></head>" \
						"<body><h1>405 Method Not Allowed</h1>" \
						"<p>The requested method is not allowed for the requested URL.</p></body></html>"

#define ERROR_PAGE_406	"<html><head><title>406 Not Acceptable</title></head>" \
						"<body><h1>406 Not Acceptable</h1>" \
						"<p>The requested resource is capable of generating only content not acceptable " \
						"according to the Accept headers sent in the request.</p></body></html>"

#define ERROR_PAGE_408	"<html><head><title>408 Request Time Out</title></head>" \
						"<body><h1>408 Request Timeout</h1>" \
						"<p>The request could not be fully received and responded in time</p></body></html>"

#define ERROR_PAGE_409	"<html><head><title>409 Conflict</title></head>" \
						"<body><h1>409 Conflict</h1>" \
						"<p>The request could not be completed due to a conflict with the current state of the resource.</p></body></html>"


#define ERROR_PAGE_413	"<html><head><title>413 Payload Too Large</title></head>" \
						"<body><h1>413 Payload Too Large</h1>" \
						"<p>The request is larger than the server is willing or able to process.</p></body></html>"


#define ERROR_PAGE_500	"<html><head><title>500 Internal Server Error</title></head>" \
						"<body><h1>500 Internal Server Error</h1>" \
						"<p>The server encountered an unexpected condition.</p></body></html>"

#define ERROR_PAGE_501	"<html><head><title>501 Not Implemented</title></head>" \
						"<body><h1>501 Not Implemented</h1>" \
						"<p>This method is not supported by the server.</p></body></html>"

#define ERROR_PAGE_505	"<html><head><title>505 HTTP Version Not Supported</title></head>" \
						"<body><h1>505 HTTP Version Not Supported</h1>" \
						"<p>The server does not support the HTTP protocol version used in the request.</p></body></html>"

#define LISTDIR_HEADER "<!DOCTYPE html>\n<html><body>\n"
#define LISTDIR_PREFIX "  "
#define LISTDIR_SUFFIX "<br>\n"
#define LISTDIR_ENDING "</body>\n</html>"

#define NULL_CHUNK "0\r\n\r\n"

// Configuration printing helpers
#define WIDTH				70UL
#define BORDER_COLOR		BOLD_PURPLE
#define SECTION_COLOR		BOLD_RED
#define SECTION_SUB_COLOR	BLUE
#define FIELD_NAME_COLOR	BOLD
#define	FIELD_VALUE_COLOR	RESET

#ifndef LOG_LEVEL
# define LOG_LEVEL 2
#endif

enum logLevel
{
	DEBUG,
	INFO,
	WARN,
	ERROR,
	NONE
};

static std::string	logColors[4] = {BLUE, GREEN, ORANGE, RED};
static std::string	logLevelStr[4] = {"DEBUG", "INFO", "WARN", "ERROR"};

enum	Method
{
	UNKNOWN	= 0,
	GET		= 1 << 0,
	POST	= 1 << 1,
	DELETE	= 1 << 2
};

enum	ResourceStatus
{
	EXISTS				= 1 << 0,
	IS_DIR				= 1 << 1,
	IS_CGI				= 1 << 2,
	PERM_ROK			= 1 << 3,
	PERM_WOK			= 1 << 4,
	PERM_XOK			= 1 << 5,
	IS_REDIRECT			= 1 << 6,
	ACCEPT_ERROR		= 1 << 7,
	CGI_FORBIDDEN		= 1 << 8,
	IS_HIDDEN			= 1 << 9,
	UPLOAD_FORBIDDEN	= 1 << 10
};

enum ContentType
{
		FTYPE_NONE		= 0,	// 0 0 0 0 0 0 0 0
		FTYPE_HTML		= 1,	// 0 0 0 0 0 0 0 1
		FTYPE_PLAIN		= 2,	// 0 0 0 0 0 0 1 0
		FTYPE_CSS		= 4,	// 0 0 0 0 0 1 0 0
		FTYPE_TEXT		= 7,	// 0 0 0 0 0 1 1 1
		FTYPE_JPEG		= 8,	// 0 0 0 0 1 0 0 0
		FTYPE_PNG		= 16,	// 0 0 0 1 0 0 0 0
		FTYPE_SVG		= 32,	// 0 0 1 0 0 0 0 0
		FTYPE_IMAGE		= 56,	// 0 0 1 1 1 0 0 0
		FTYPE_CGI_PY	= 64,	// 0 1 0 0 0 0 0 0
		FTYPE_CGI_PL	= 128,	// 1 0 0 0 0 0 0 0
		FTYPE_IS_CGI	= 192,	// 1 1 0 0 0 0 0 0
		FTYPE_ANY		= 255	// 1 1 1 1 1 1 1 1
};

enum	HttpStatus
{
	HTTP_UNKNOWN_STATUS = 0,
	HTTP_OK = 200,
	HTTP_CREATED = 201,
	HTTP_NO_CONTENT = 204,
	HTTP_REDIRECT_PERM = 301,
	HTTP_REDIRECT_TEMP = 302,
	HTTP_BAD_REQUEST = 400,
	HTTP_FORBIDDEN = 403,
	HTTP_NOT_FOUND = 404,
	HTTP_METHOD_NOT_ALLOWED = 405,
	HTTP_NOT_ACCEPTABLE = 406,
	HTTP_TIMEOUT = 408,
	HTTP_CONFLICT = 409,
	HTTP_CONTENT_TOO_LARGE = 413,
	HTTP_INTERNAL_SERVER_ERROR = 500,
	HTTP_NOT_IMPLEMENTED = 501,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

enum	RequestStage
{
	TRY_ACCEPTING,
	INIT,
	READING_HEADER,
	READING_BODY,
	PROCESSING_REQUEST,
	CGI_RUNNING,
	SENDING_STRING,
	SENDING_FILE,
	ABORTING,
	DONE
};

enum	ProcessError
{
	ERR_NONE,
	WOULD_BLOCK,
	KILL_REQUEST,
	KILL_CLIENT,
	KILL_SERVER
};

typedef struct s_PollRevent
{
	bool	error;
	bool	server;
	short	revent;
	int		client_id;
}	PollRevent;

inline Method			operator|(Method a, Method b) { return static_cast<Method>(static_cast<int>(a) | static_cast<int>(b)); }
inline Method&			operator|=(Method& a, Method b) { a = a | b; return a; }
inline ResourceStatus	operator|(ResourceStatus a, ResourceStatus b) { return static_cast<ResourceStatus>(static_cast<int>(a) | static_cast<int>(b)); }
inline ResourceStatus&	operator|=(ResourceStatus& a, ResourceStatus b) { a = a | b; return a; }
inline ResourceStatus	operator&(ResourceStatus a, ResourceStatus b) { return static_cast<ResourceStatus>(static_cast<int>(a) & static_cast<int>(b)); }
inline ResourceStatus&	operator&=(ResourceStatus& a, ResourceStatus b) { a = a & b; return a; }
