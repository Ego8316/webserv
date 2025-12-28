# 🌐 WebServ

A fully functional HTTP/1.1 web server implementation in C++98. This project provides deep insight into the HTTP protocol, non-blocking I/O, and the architecture of web servers like NGINX.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
  - [Core Features](#core-features)
  - [Bonus Features](#bonus-features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
  - [Running the Server](#running-the-server)
  - [Configuration File](#configuration-file)
  - [Testing](#testing)
- [Implementation Details](#implementation-details)
  - [Architecture](#architecture)
  - [Non-Blocking I/O](#non-blocking-io)
  - [Request Processing Pipeline](#request-processing-pipeline)
  - [CGI Execution](#cgi-execution)
- [Configuration](#configuration)
- [Project Structure](#project-structure)
- [Log Levels](#log-levels)
- [Team](#team)

## 🔍 Overview

WebServ is a custom HTTP/1.1 server implementation that handles multiple concurrent connections using non-blocking I/O with `poll()` (or equivalent). The server is fully compliant with modern web browsers and supports static file serving, file uploads, CGI execution, and more.

This project demonstrates:
- Deep understanding of the HTTP protocol
- Non-blocking socket programming
- Event-driven architecture
- Configuration parsing
- Process management for CGI
- Error handling and resilience

## ✨ Features

### Core Features

- **HTTP/1.1 Protocol**: Full implementation of HTTP/1.1 with proper status codes
- **Multiple HTTP Methods**: Support for GET, POST, and DELETE
- **Non-Blocking I/O**: Single `poll()` event loop handling all client connections
- **Keep-Alive Connections**: Persistent connections for improved performance
- **Chunked Transfer Encoding**: Support for chunked request bodies
- **Static File Serving**: Serve HTML, CSS, JavaScript, images, and other static content
- **File Upload**: Accept file uploads from clients via POST requests
- **Directory Listing**: Automatic directory index generation (when enabled)
- **CGI Support**: Execute CGI scripts (PHP, Python, etc.) based on file extensions
- **Custom Error Pages**: Configurable error pages for different HTTP status codes
- **Multiple Virtual Servers**: Listen on multiple ports with different configurations
- **Route-Based Configuration**: Per-location configuration (similar to NGINX)
- **HTTP Redirections**: Support for 301/302 redirects
- **Request Size Limits**: Configurable maximum body size and buffer sizes
- **Timeout Handling**: Automatic client timeout management

### Bonus Features

- **Cookie Management**: Parse and set HTTP cookies
- **Session Management**: Simple session support with cookie-based tracking
- **Multiple CGI Types**: Handle various CGI implementations (PHP-CGI, Python, etc.)

## 🚀 Getting Started

### Prerequisites

- **C++ Compiler**: GCC or Clang with C++98 support
- **Make**: Build automation tool
- **Optional**: PHP-CGI, Python, or other CGI interpreters for dynamic content

### Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   ```

2. Navigate to the project directory:
   ```bash
   cd Docker_webserv/WebServ
   ```

3. Compile the project:
   ```bash
   make
   ```

   Optional: Compile with custom log level (0-4, default is 2):
   ```bash
   make LOG_LEVEL=3
   ```

## 🖥️ Usage

### Running the Server

Start the server with a configuration file:

```bash
./webserv config.conf
```

The server will:
1. Parse the configuration file
2. Set up listening sockets on specified ports
3. Enter the main event loop
4. Handle incoming connections and requests

Press `Ctrl+C` to gracefully shut down the server.

### Configuration File

The configuration file uses NGINX-inspired syntax. Example:

```nginx
server {
    listen 0.0.0.0:8080;
    server_name webserv.local;
    root ./www/site1;
    index index.html;
    
    client_max_body_size 15M;
    timeout 3s;
    
    error_pages {
        404 /errors/404.html;
        500 /errors/500.html;
    }
    
    location / {
        root ./www/site1;
        limit_except GET;
        autoindex off;
    }
    
    location /upload {
        root ./www/site1/uploads;
        upload_path ./www/site1/uploads;
        limit_except POST;
    }
    
    location /cgi-bin {
        root ./www/site1/cgi-bin;
        limit_except POST GET;
        cgi on;
    }
    
    location /redirect {
        return 301 https://example.com;
    }
}
```

### Testing

**Browser Testing:**
```bash
# Start the server
./webserv config.conf

# Open in browser
open http://localhost:8080
```

**Command Line Testing:**
```bash
# Simple GET request
curl http://localhost:8080/

# POST with data
curl -X POST -d "data=test" http://localhost:8080/upload

# Upload a file
curl -X POST -F "file=@test.txt" http://localhost:8080/upload

# Test chunked encoding
curl -X POST -H "Transfer-Encoding: chunked" \
  --data-binary @file.txt http://localhost:8080/upload
```

**Using telnet:**
```bash
telnet localhost 8080
GET / HTTP/1.1
Host: localhost

```

## 🔧 Implementation Details

### Architecture

WebServ follows a modular, event-driven architecture:

```
┌─────────────┐
│   WebServ   │  ◄─── High-level server coordinator
└──────┬──────┘
       │
       ├─► ServerCore    ◄─── Socket management & poll() wrapper
       │
       ├─► Client        ◄─── Per-client state machine
       │      │
       │      ├─► Request        ◄─── HTTP request parsing
       │      ├─► Response       ◄─── HTTP response building
       │      └─► RequestHandler ◄─── Request routing & processing
       │
       └─► ServerConfig  ◄─── Configuration management
```

**Key Components:**

- **WebServ**: Manages multiple clients and coordinates the event loop
- **ServerCore**: Handles low-level socket operations and `poll()` management
- **Client**: Implements a state machine for request/response lifecycle
- **Request**: Parses and stores HTTP request data
- **Response**: Builds HTTP responses
- **RequestHandler**: Routes requests and handles different content types
- **CGI**: Manages CGI process execution with pipes
- **ConfigParser**: Parses NGINX-style configuration files

### Non-Blocking I/O

The server uses a single `poll()` call to monitor all file descriptors:

- **Server socket**: Accepts new connections
- **Client sockets**: Read requests and write responses
- **CGI pipes**: Communicate with CGI processes

All file descriptors are set to non-blocking mode (on macOS using `fcntl()` with `O_NONBLOCK`).

```cpp
// Event loop pseudocode
while (!shutdown) {
    poll(all_fds);
    
    if (server_fd_readable)
        accept_new_client();
    
    for each client_fd_readable:
        read_request();
    
    for each client_fd_writable:
        write_response();
}
```

### Request Processing Pipeline

Each client follows a state machine through these stages:

1. **TRY_ACCEPTING**: Accept the connection
2. **INIT**: Initialize request/response objects
3. **READING_HEADER**: Read HTTP headers
4. **READING_BODY**: Read HTTP body (if present)
5. **PROCESSING_REQUEST**: Route and handle the request
6. **CGI_RUNNING**: Wait for CGI completion (if applicable)
7. **SENDING_STRING**: Send response headers and buffered content
8. **SENDING_FILE**: Stream file content to client
9. **DONE**: Complete or prepare for next request (keep-alive)

### CGI Execution

CGI scripts are executed in separate processes using `fork()` and `execve()`:

1. Create pipes for stdin/stdout communication
2. Fork a child process
3. Set up environment variables (REQUEST_METHOD, QUERY_STRING, etc.)
4. Execute the CGI script
5. Monitor the pipes in the main event loop
6. Collect output and return to client

## 📝 Configuration

**Server Block Directives:**

- `listen <host>:<port>`: Bind address and port
- `server_name <name>`: Server identifier
- `root <path>`: Document root directory
- `index <file>`: Default index file
- `autoindex on|off`: Enable/disable directory listing
- `client_max_body_size <size>`: Maximum request body size
- `timeout <duration>`: Client timeout
- `error_pages { <code> <file>; }`: Custom error pages

**Location Block Directives:**

- `root <path>`: Override document root
- `index <file>`: Default file for directories
- `autoindex on|off`: Directory listing
- `limit_except <methods>`: Allowed HTTP methods
- `cgi on|off`: Enable CGI execution
- `upload_path <path>`: File upload destination
- `return <code> <url>`: HTTP redirect

## 📁 Project Structure

```
WebServ/
├── bin/                    # Compiled executable
├── include/                # Header files
│   ├── WebServ.hpp         # Main server class
│   ├── ServerCore.hpp      # Socket & poll management
│   ├── Client.hpp          # Client state machine
│   ├── Request.hpp         # HTTP request parser
│   ├── Response.hpp        # HTTP response builder
│   ├── RequestHandler.hpp  # Request routing
│   ├── CGI.hpp             # CGI execution
│   ├── Cookie.hpp          # Cookie management
│   ├── ServerConfig.hpp    # Configuration structures
│   ├── ConfigParser.hpp    # Config file parser
│   └── headers.hpp         # Common includes
├── srcs/                   # Source files
│   ├── main.cpp            # Entry point
│   ├── WebServ.cpp
│   ├── ServerCore.cpp
│   ├── Client.cpp
│   ├── Request.cpp
│   ├── Response.cpp
│   ├── RequestHandler.cpp
│   ├── Resource.cpp
│   ├── CGI.cpp
│   ├── Cookie.cpp
│   ├── ConfigLexer.cpp
│   ├── ConfigParser.cpp
│   ├── ConfigInterpreter.cpp
│   ├── ServerConfig.cpp
│   └── utils.cpp
├── www/                    # Web content
│   ├── site1/              # Example website
│   ├── site2/              # Another site
│   └── site3/              # Yet another site
├── objs/                   # Object files
├── config.conf             # Example configuration
├── Makefile                # Build instructions
├── WebServ.pdf             # Project subject
└── README.md               # This file
```

## 📊 Log Levels

Control logging verbosity using the `LOG_LEVEL` make variable:

```bash
make LOG_LEVEL=<level>
```

**Levels:**
- **0**: Silent (no logs)
- **1**: ERROR only
- **2**: ERROR + WARN (default)
- **3**: ERROR + WARN + INFO
- **4**: ERROR + WARN + INFO + DEBUG

Example:
```bash
make clean
make LOG_LEVEL=4  # Verbose debugging
./webserv config.conf
```

## 👥 Team

This project was developed by:

- [Victor Viterbo](https://github.com/victorviterbo) - vviterbo@student.42.fr
- [Hugo Cavet](https://github.com/hcavet) - hcavet@student.42lausanne.ch

---

**Project Grade**: Pending evaluation

**Developed as part of the 42 curriculum** - A deep dive into HTTP protocol implementation and network programming.

## 📚 Resources

- [RFC 7230-7235](https://tools.ietf.org/html/rfc7230): HTTP/1.1 Specification
- [RFC 3875](https://tools.ietf.org/html/rfc3875): CGI Specification
- [NGINX Documentation](https://nginx.org/en/docs/): Configuration inspiration
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/): Socket programming reference

## 🐛 Known Limitations

- HTTP/1.1 only (no HTTP/2 support)
- Limited to standard CGI (no FastCGI)
- No SSL/TLS support
- Virtual hosts not implemented (though multiple ports are supported)
- No compression (gzip, etc.)

## 🔒 Security Considerations

This is an educational project. For production use, consider:
- Path traversal protection is basic
- No rate limiting
- No DDoS protection
- CGI processes inherit server privileges
- Input validation should be enhanced
