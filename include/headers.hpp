/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:15:31 by ego               #+#    #+#             */
/*   Updated: 2025/10/02 21:23:11 by ego              ###   ########.fr       */
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

#define SERV_ERROR -1
#define NEW_CLIENT 0
#define CLIENT_ERR_IDX(idx) (-idx - 1)

#define NO_TIMEOUT -1
#define BUFFER_SIZE 1024
#define CLIENT_LIMIT 1000
#define MAX_COOKIE_SESSIONS 1000
//#define COOKIE_LIFE_TIME 3600

#define RESOURCE_NOT_FOUND 0
#define PERM_ISSUE 1
#define IS_DIR 2
#define PERM_ROK 4
#define PERM_WOK 8
#define PERM_XOK 16
#define IS_CGI 32
#define EXISTS 128

// Default error pages
#define ERROR_PAGE_400 "<html><head><title>400 Bad Request</title></head>" \
                       "<body><h1>400 Bad Request</h1>" \
                       "<p>Your browser sent a request that this server could not understand.</p></body></html>"

#define ERROR_PAGE_403 "<html><head><title>403 Forbidden</title></head>" \
                       "<body><h1>403 Forbidden</h1>" \
                       "<p>You don't have permission to access this resource.</p></body></html>"

#define ERROR_PAGE_404 "<html><head><title>404 Not Found</title></head>" \
                       "<body><h1>404 Not Found</h1>" \
                       "<p>The requested URL was not found on this server.</p></body></html>"

#define ERROR_PAGE_500 "<html><head><title>500 Internal Server Error</title></head>" \
                       "<body><h1>500 Internal Server Error</h1>" \
                       "<p>The server encountered an unexpected condition.</p></body></html>"

#define ERROR_PAGE_501 "<html><head><title>501 Not Implemented</title></head>" \
                       "<body><h1>501 Not Implemented</h1>" \
                       "<p>This method is not supported by the server.</p></body></html>"

#define ERROR_PAGE_505 "<html><head><title>505 HTTP Version Not Supported</title></head>" \
                       "<body><h1>505 HTTP Version Not Supported</h1>" \
                       "<p>The server does not support the HTTP protocol version used in the request.</p></body></html>"


enum	Method
{
	GET,
	POST,
	DELETE,
	CGI_RUN,
	ERROR
};

enum	ParseError
{
	NONE,
	UNSUPPORTED_METHOD,
	INVALID_REQUEST_LINE,
	INVALID_HEADER,
	BAD_CONTENT_LENGTH,
	UNREADABLE_FILE
};

enum	ContentTypes
{
	HTML,
	PLAIN,
	JPEG,
	PNG,
	CGI_PY,
	CGI_PHP
};

enum	HttpStatus
{
	OK = 200,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	HTTP_VERSION_NOT_SUPPORTED = 505
};
