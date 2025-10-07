/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:15:31 by ego               #+#    #+#             */
/*   Updated: 2025/10/07 16:39:44 by victorviter      ###   ########.fr       */
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

#define SERV_ERROR -1
#define NEW_CLIENT 0
#define CLIENT_ERR_IDX(idx) (-idx - 1)

#define NO_TIMEOUT -1
#define BUFFER_SIZE 1024
#define CLIENT_LIMIT 1000
#define MAX_COOKIE_SESSIONS 1000
//#define COOKIE_LIFE_TIME 3600

#define NOT_FOUND 0
#define PERM_ISSUE 1
#define IS_DIR 2
#define PERM_ROK 4
#define PERM_WOK 8
#define PERM_XOK 16
#define IS_CGI 32
#define EXISTS 128

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

enum ContentTypes
{
	HTML,
	PLAIN,
	JPEG,
	PNG,
	CGI_PY,
	CGI_PHP
};
