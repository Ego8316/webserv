/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:15:31 by ego               #+#    #+#             */
/*   Updated: 2025/09/28 19:03:28 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>

#define SERV_ERROR -1
#define NEW_CLIENT 0
#define CLIENT_ERR_IDX(idx) (-(idx + 2))

#define ALL_FDS -1
#define DEFAULT_EVENT -1
#define NO_TIMEOUT -1
#define SERVER_IDX 0

#define BUFFER_SIZE 1024
#define REQUEST_TIME_WAIT 100000

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

enum ContentTypes
{
	HTML,
	PLAIN,
	JPEG,
	PNG
};

struct Config
{
	int		port_number;
	int		backlog;
	int		time_out;
	int		client_limit;
	int		domain;
	int		type;
	int		protocol;
};
