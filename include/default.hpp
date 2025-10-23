/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   default.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 22:53:55 by ego               #+#    #+#             */
/*   Updated: 2025/10/23 23:31:28 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define MIN_PORT						0
#define MAX_PORT 						65535
#define DEFAULT_PORT					8080

#define MIN_MAX_HEADER_SIZE 			0UL
#define MAX_MAX_HEADER_SIZE				1048576UL
#define DEFAULT_MAX_HEADER_SIZE			1000UL

#define MIN_MAX_BODY_SIZE				0UL
#define MAX_MAX_BODY_SIZE				10485760UL
#define DEFAULT_MAX_BODY_SIZE			10000UL

#define MIN_CLIENT_LIMIT				1
#define MAX_CLIENT_LIMIT				1024
#define DEFAULT_CLIENT_LIMIT			100

#define MIN_PROCESSING_TIME_LIMIT		1L
#define MAX_PROCESSING_TIME_LIMIT		10000L
#define DEFAULT_PROCESSING_TIME_LIMIT	100L

#define MIN_MAX_REQUEST_TIME			100L
#define MAX_MAX_REQUEST_TIME			1000000L
#define DEFAULT_MAX_REQUEST_TIME		10000L

#define MIN_INCOMING_QUEUE_BACKLOG		1
#define MAX_INCOMING_QUEUE_BACKLOG		1024
#define DEFAULT_INCOMING_QUEUE_BACKLOG	100

#define MIN_BUFFER_SIZE					1UL
#define MAX_BUFFER_SIZE					65536UL
#define DEFAULT_BUFFER_SIZE				1024UL

#define MIN_COOKIE_SESSIONS_MAX			0
#define MAX_COOKIE_SESSIONS_MAX			1000000
#define DEFAULT_COOKIE_SESSIONS_MAX		1000

#define MIN_COOKIE_LIFETIME				0
#define MAX_COOKIE_LIFETIME				1000000
#define DEFAULT_MAX_COOKIE_LIFETIME		1000

#define DEFAULT_SERVER_HOME				"/var/www"
#define DEFAULT_ENABLE_LISTDIR			0
#define DEFAULT_DEFAULT_PAGE			"index.html"