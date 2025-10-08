/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:15:31 by ego               #+#    #+#             */
/*   Updated: 2025/10/09 00:58:58 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include <algorithm>
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
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <dirent.h>

#define SERV_ERROR -1
#define NEW_CLIENT 1

#define NO_TIMEOUT -1
#define BUFFER_SIZE 1024
#define CLIENT_LIMIT 1000
#define MAX_COOKIE_SESSIONS 1000

enum Method
{
	GET,
	POST,
	DELETE,
	CGI_RUN,
	ERROR
};

enum ParseError
{
	NONE,
	UNSUPPORTED_METHOD,
	INVALID_REQUEST_LINE,
	INVALID_HEADER,
	BAD_CONTENT_LENGTH,
	UNREADABLE_FILE
};

enum FileStatus
{
	NOT_FOUND	= 0,	// 0 0 0 0 0 0 0 0
	PERM_ISSUE	= 1,	// 0 0 0 0 0 0 0 1
	IS_DIR		= 2,	// 0 0 0 0 0 0 1 0
	PERM_ROK	= 4,	// 0 0 0 0 0 1 0 0
	PERM_WOK	= 8,	// 0 0 0 0 1 0 0 0
	PERM_XOK	= 16,	// 0 0 0 1 0 0 0 0
	IS_CGI		= 32,	// 0 0 1 0 0 0 0 0
	EXISTS		= 128	// 1 0 0 0 0 0 0 0
};

enum ContentTypes
{
		ACCEPT_NONE		= 0,	// 0 0 0 0 0 0 0 0
		ACCEPT_HTML		= 1,	// 0 0 0 0 0 0 0 1
		ACCEPT_PLAIN	= 2,	// 0 0 0 0 0 0 1 0
		// 0 0 0 0 0 1 0 0
		ACCEPT_TEXT		= 7,	// 0 0 0 0 0 1 1 1
		ACCEPT_JPEG		= 8,	// 0 0 0 0 1 0 0 0
		ACCEPT_PNG		= 16,	// 0 0 0 1 0 0 0 0
		// 0 0 1 0 0 0 0 0
		ACCEPT_IMAGE	= 56,	// 0 0 1 1 1 0 0 
		ACCEPT_CGI_PY	= 64,	// 0 1 0 0 0 0 0 0
		ACCEPT_CGI_PHP	= 128,	// 1 0 0 0 0 0 0 0
		ACCEPT_ANY		= 255	// 1 1 1 1 1 1 1 1
};

typedef struct s_pollRevent
{
	bool	is_error;
	short	revent;
	int		client_id;
}	pollRevent;

#define LISTDIR_HEADER "<!DOCTYPE html>\n<html><body>\n"
#define LISTDIR_PREFIX "  "
#define LISTDIR_SUFFIX "<br>\n"
#define LISTDIR_ENDING "</body>\n</html>"