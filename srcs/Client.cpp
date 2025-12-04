/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vviterbo <vviterbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/12/04 15:40:41 by vviterbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "headers.hpp"

extern int	g_shutdown;

/**
 * @brief Builds a client object attached to a server and config.
 *
 * @param config Server configuration.
 * @param server Owning server core.
 */
Client::Client(const ServerConfig *config, ServerCore *server)
{
	this->_config = config;
	this->_server = server;
	this->_client_fd = -1;
	std::memset(&this->_client_addr, 0, sizeof(struct sockaddr_in));
	this->_client_len = sizeof(struct sockaddr_in);
	this->_client_id = 0;
	this->_state = TRY_ACCEPTING;
	this->_error = ERR_NONE;
	this->_leftover = "";
	this->_bytes_sent = 0;
	this->_bytes_in_buffer = 0;
	this->_keep_alive = true;
	this->_request = NULL;
	this->_response = NULL;
	this->_time_limit = utils::getTime() + config->timeout;
}

/**
 * @brief Copy constructor.
 *
 * @param other Source client.
 */
Client::Client(const Client &other)
{
	*this = other;
}

/**
 * @brief Assignment operator.
 *
 * @param other Source client.
 *
 * @return Reference to this client.
 */
Client	&Client::operator=(const Client &other)
{
	if (this != &other)
	{
		this->_config = other._config;
		this->_server = other._server;
		this->_client_fd = other._client_fd;
		this->_client_addr = other._client_addr;
		this->_client_len = other._client_len;
		this->_client_id = other._client_id;
		this->_state = other._state;
		this->_error = other._error;
		this->_leftover = other._leftover;
		this->_bytes_sent = other._bytes_sent;
		this->_bytes_in_buffer = other._bytes_in_buffer;
		this->_keep_alive = other._keep_alive;
		if (this->_request)
			delete this->_request;
		this->_request = other._request ? new Request(*other._request) : NULL;
		if (this->_response)
			delete this->_response;
		this->_response = other._response ? new Response(*other._response) : NULL;
	}
	return (*this);
}

/**
 * @brief Destructor closing socket and freeing request/response.
 */
Client::~Client()
{
	if (this->_client_fd > 0)
	{
		close(this->_client_fd);
		this->_client_fd = -1;
	}
	if (this->_request)
		delete _request;
	if (this->_response)
		delete _response;
	this->_server->pollRemove(this->_client_id);
}

/**
 * @brief Returns client sockaddr storage.
 *
 * @return Reference to sockaddr_in.
 */
struct sockaddr_in	&Client::getClientAddr()
{
	return (this->_client_addr);
}

/**
 * @brief Returns stored sockaddr length reference.
 *
 * @return Reference to socklen_t.
 */
socklen_t	&Client::getClientLen()
{
	return (this->_client_len);
}

/**
 * @brief Returns client identifier.
 *
 * @return Client id.
 */
int	Client::getId() const
{
	return (this->_client_id);
}

/**
 * @brief Returns client socket descriptor.
 *
 * @return Socket fd.
 */
int	Client::getFd()
{
	return (this->_client_fd);
}

/**
 * @brief Returns current processing state.
 *
 * @return RequestStage value.
 */
RequestStage	Client::getState()
{
	return (this->_state);
}

/**
 * @brief Returns reference to owning ServerCore.
 *
 * @return ServerCore reference.
 */
ServerCore	&Client::getServer()
{
	return (*this->_server);
}

/**
 * @brief Returns per-iteration processing time limit.
 *
 * @return Time limit in seconds.
 */
long	Client::getTimeLimit()
{
	return (this->_time_limit);
}

/**
 * @brief Sets the time limit for the current request
 */
void	Client::setTimeLimit(long value)
{
	this->_time_limit = value;
}

/**
 * @brief Sets client identifier.
 *
 * @param id New client id.
 */
void	Client::setClientId(int id)
{
	this->_client_id = id;
}

/**
 * @brief Stores accepted socket descriptor.
 *
 * @param fd Socket descriptor.
 */
void	Client::setFd(int fd)
{
	this->_client_fd = fd;
}

/**
 * @brief Updates state machine stage.
 *
 * @param state New state.
 */
void	Client::setState(RequestStage state)
{
	this->_state = state;
}

Response	*Client::getResponse()
{
	return(this->_response);
}

/**
 * @brief Drives the client state machine for one poll cycle.
 *
 * @return ProcessError code (or SERV_ERROR).
 */
int	Client::handleEvent()
{
	this->_error = ERR_NONE;
	printState();
	if (g_shutdown)
		return (KILL_SERVER);
	if (this->_state == TRY_ACCEPTING)
		return (this->_tryAccepting());
	if (this->_state == ABORTING)
		return (SERV_ERROR);
	else if (this->_error == WOULD_BLOCK)
		return (ERR_NONE);
	else if (this->_state == DONE)
		return (ERR_NONE);
	if (this->_state == INIT)
		this->_requestInit();
	if (this->_state == READING_HEADER && this->_readHeader() == SERV_ERROR)
		return (SERV_ERROR);
	if (this->_state == READING_BODY && this->_readBody() == SERV_ERROR)
		return (SERV_ERROR);
	if (_state == PROCESSING_REQUEST)
		this->_processRequest();
	if (_state == CGI_RUNNING )
		this->_monitorCGI();
	if (this->_state == SENDING_STRING && this->_sendString() == SERV_ERROR)
		return (SERV_ERROR);
	if (this->_state == SENDING_FILE && this->_sendFile() == SERV_ERROR)
		return (SERV_ERROR);
	if (_state == ABORTING)
		_error = KILL_CLIENT;
	if (this->_state != TRY_ACCEPTING && this->_response->getHttpStatus() == HTTP_BAD_REQUEST)
		_error = KILL_CLIENT;
	if (_state == DONE || _error > WOULD_BLOCK)
	{
		delete this->_request;
		this->_request = NULL;
		delete this->_response;
		this->_response = NULL;
	}
	return (_error);
}

/**
 * @brief Accepts a pending connection and registers it with poll.
 *
 * @return 0 on success, SERV_ERROR or WBLOCK otherwise.
 */
int	Client::_tryAccepting()
{
	if (this->_server->socketAcceptClient(this) == SERV_ERROR)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return (this->_error = WOULD_BLOCK, WBLOCK);
		this->_state = ABORTING;
		return (SERV_ERROR);
	}
	if (this->_server->pollAvailFor(this->_client_id, POLLIN))
		this->_state = INIT;
	else
		this->_state = DONE;
	this->_server->pollAdd(this->getFd(), POLLIN | POLLOUT, this->_client_id);
	utils::logMsg("INFO", GREEN, "Accepted new connection", this->_client_id);
	this->printState();
	return (0);
}

/**
 * @brief Allocates Request/Response objects and primes header reading.
 *
 * @return 0 on success.
 */
int	Client::_requestInit()
{
	this->_request = new Request();
	this->_response = new Response();
	this->_bytes_sent = 0;
	this->_bytes_in_buffer = 0;
	this->_keep_alive = true;
	_state = READING_HEADER;
	return (0);
}


/**
 * @brief Reads the HTTP header, advancing state when complete.
 *
 * @return 0, SERV_ERROR, or WOULD_BLOCK.
 */
int	Client::_readHeader()
{
	std::vector<char>	buffer(this->_config->client_header_buffer_size);
	std::string			&header_str = this->_request->getRawHeader();
	size_t				pos;

	if (!this->_leftover.empty())
	{
		utils::logMsg("DEBUG", CYAN, "[_readHeader] Applying leftover (" + utils::toString(_leftover.size()) + " bytes)", this->_client_id);
		header_str.append(this->_leftover);
		this->_leftover.clear();
	}
	ssize_t	bytes_read = this->_server->socketRead(&buffer[0], buffer.size(), this);
	if (bytes_read == SERV_ERROR)
		return (SERV_ERROR);
	if (bytes_read == WBLOCK)
	{
		this->_error = WOULD_BLOCK;
		return (WOULD_BLOCK);
	}
	if (bytes_read == 0)
	{
		utils::logMsg("WARN", ORANGE, "[_readHeader] Client closed the connection", this->_client_id);
		return (this->_state = ABORTING, 0);
	}
	utils::logMsg("INFO", GREEN, "[_readHeader] Read " + utils::toString(bytes_read) + " bytes", this->_client_id);
	header_str.append(buffer.begin(), buffer.begin() + bytes_read);
	if (header_str.size() > this->_config->client_header_buffer_size)
		return (this->_state = ABORTING, SERV_ERROR);
	if ((pos = header_str.find("\r\n\r\n")) == std::string::npos)
	{
		utils::logMsg("DEBUG", CYAN, "[_readHeader] Incomplete header — waiting for more", this->_client_id);
		return (0);
	}
	this->_leftover = header_str.substr(pos + 4);
	header_str.erase(pos);
	utils::logMsg("INFO", GREEN, "[_readHeader] Header fully received (" + utils::toString(header_str.size()) + " bytes)", this->_client_id);
	this->_request->parseHeader(*this->_config);
	if (!this->_request->isChunked() && this->_request->getContentLength() == 0)
	{
		this->_state = PROCESSING_REQUEST;
		printState();
		return (0);
	}
	this->_state = READING_BODY;
	printState();
	return (0);
}

/**
 * @brief Reads the HTTP body, handling both content-length and chunked.
 *
 * @return 0, SERV_ERROR, or WOULD_BLOCK.
 */
int	Client::_readBody()
{
	std::vector<char>	buffer(this->_config->client_body_buffer_size);
	std::string			&body_str = this->_request->getRawBody();
	size_t				pos;
	
	if (body_str.empty() && !this->_request->isChunked())
		body_str.reserve(this->_request->getContentLength());
	if (!this->_leftover.empty())
	{
		utils::logMsg("DEBUG", CYAN, "[_readBody] Applying leftover (" + utils::toString(_leftover.size()) + " bytes)", this->_client_id);
		body_str.append(this->_leftover);
		this->_leftover.clear();
	}
	if (!(body_str.size() >= this->_request->getContentLength())
		&& !(this->_request->isChunked() && (pos = body_str.find(NULL_CHUNK)) != std::string::npos))
	{
		ssize_t	bytes_read = this->_server->socketRead(&buffer[0], buffer.size(), this);
		if (bytes_read == SERV_ERROR)
			return (SERV_ERROR);
		if (bytes_read == WBLOCK)
		{
			this->_error = WOULD_BLOCK;
			return (WOULD_BLOCK);
		}
		if (bytes_read == 0)
		{
			utils::logMsg("WARN", ORANGE, "[_readBody] Client closed the connection", this->_client_id);
			return (this->_state = ABORTING, 0);
		}
		body_str.append(buffer.begin(), buffer.begin() + bytes_read);
		utils::logMsg("INFO", GREEN, "[_readBody] Read " + utils::toString(bytes_read) + " bytes (total: " + utils::toString(body_str.size()) + ")", this->_client_id);
	}
	if (body_str.size() >= this->_request->getContentLength())
	{
		_leftover = body_str.substr(this->_request->getContentLength());
		body_str.erase(this->_request->getContentLength());
		utils::logMsg("INFO", GREEN, "[_readBody] Body complete (" + utils::toString(body_str.size()) + " bytes)", this->_client_id);
		this->_state = PROCESSING_REQUEST;
		printState();
		return (0);
	}
	if (this->_request->isChunked() && (pos = body_str.find(NULL_CHUNK)) != std::string::npos)
	{
		_leftover = body_str.substr(pos);
		body_str.erase(pos);
		this->_request->unchunkBody();
		utils::logMsg("INFO", GREEN, "[_readBody] Chunked body complete", this->_client_id);
		this->_state = PROCESSING_REQUEST;
		printState();
		return (0);
	}
	utils::logMsg("DEBUG", CYAN, "[_readBody] Partial body received — waiting for more", this->_client_id);
	return (0);
}

/**
 * @brief Builds the Response based on the parsed Request.
 */
void	Client::_processRequest()
{
	RequestHandler::handle(this->_response, *_request, *_config);
	utils::logMsg("INFO", GREEN, "Processing request", this->_client_id);
	std::string conn = utils::toLower(_request->headerGetField("Connection"));
	if (_request->getVersion() == "HTTP/1.0")
		_keep_alive = false;
	else
		_keep_alive = (conn.find("close") == std::string::npos);
	_response->setHeaders("Connection", _keep_alive ? "keep-alive" : "close");
	if (_response->isCGI())
		this->_state = CGI_RUNNING;
	else
		this->_state = SENDING_STRING;
	printState();
	return ;
}

/**
 * @brief Monitors CGI execution until completion.
 */
void	Client::_monitorCGI()
{
	CGI	*cgi = _response->getCGI();

	if (!cgi || cgi->isComplete())
	{
		this->_state = SENDING_STRING;
		this->printState();
		return ;
	}
	cgi->setClientId(this->_client_id);
	cgi->Run(*this, *_request, *_config, *_response, *_server);
	if (cgi->isComplete())
	{
		this->_state = SENDING_STRING;
		this->printState();
	}
}

/**
 * @brief Sends buffered response string over the socket.
 *
 * @return 0 on progress/completion, SERV_ERROR or WOULD_BLOCK otherwise.
 */
int	Client::_sendString()
{
	const std::string	&response_str = this->_response->getString();
	size_t				bytes_to_send = response_str.size() - this->_bytes_sent;

	if (bytes_to_send > 0)
	{
		if (this->_bytes_sent == 0)
			utils::logMsg("DEBUG", CYAN, "[_sendString] Response string:\n" + response_str, this->_client_id);
		int	sent = this->_server->socketWrite(response_str.c_str() + this->_bytes_sent, std::min(bytes_to_send, this->_config->client_body_buffer_size), this);
		if (sent == SERV_ERROR)
			return (SERV_ERROR);
		if (sent == WBLOCK)
			return (0);
		if (sent == 0)
		{
			utils::logMsg("WARN", ORANGE, "[_sendString] Client closed the connection", this->_client_id);
			return (this->_state = ABORTING, 0);
		}
		this->_bytes_sent += sent;
		return (0);
	}
	if (this->_response->getFd() > -1)
		this->_state = SENDING_FILE;
	else
	{
		this->_state = DONE;
		if (!_keep_alive)
			this->_error = KILL_CLIENT;
	}
	this->printState();
	return (0);
}

/**
 * @brief Streams response file descriptor contents to the client.
 *
 * @return 0 on progress/completion, SERV_ERROR or WOULD_BLOCK otherwise.
 */
int	Client::_sendFile()
{
	std::vector<char>	buffer(this->_config->client_body_buffer_size);

	ssize_t	bytes_read = read(this->_response->getFd(), &buffer[0], this->_config->client_body_buffer_size);
	if (bytes_read < 0)
		return (SERV_ERROR);
	else if (bytes_read == 0)
	{
		close(this->_response->getFd());
		this->_response->setFd(-1);
		this->_state = DONE;
		if (!_keep_alive)
			this->_error = KILL_CLIENT;
		this->printState();
		return (0);
	}
	ssize_t	bytes_sent = this->_server->socketWrite(&buffer[0], bytes_read, this);
	if (bytes_sent == SERV_ERROR)
		return (SERV_ERROR);
	if (bytes_sent == WBLOCK)
	{
		this->_error = WOULD_BLOCK;
		return (WOULD_BLOCK);
	}
	if (bytes_sent == 0)
	{
		utils::logMsg("WARN", ORANGE, "[_sendFile] Client closed the connection", this->_client_id);
		return (this->_state = ABORTING, 0);
	}
	return (0);
}

/**
 * @brief Resets request/response objects for a new exchange.
 */
void	Client::_prepareNew()
{
	delete this->_request;
	delete this->_response;
	this->_request = new Request();
	this->_response = new Response();
}

/**
 * @brief Logs the current client state.
 */
void	Client::printState() const
{
	utils::logMsg("INFO", BLUE, "State is now " + utils::stateToStr(this->_state), this->_client_id);
	return ;
}

/**
 * @brief Logs the raw HTTP header for debugging.
 */
void	Client::printHeader() const
{
	std::cout << BOLD_GRAY << "[Client " << this->_client_id << "]" << RESET
		<< GRAY << "\n\t[HEADER START]\n"
		<< this->_request->getRawHeader()
		<< "\n\t[HEADER END]" << RESET << std::endl;
	return ;
}

/**
 * @brief Logs the parsed request for debugging.
 */
void	Client::printRequest() const
{
	std::cout << BOLD_GRAY << "[Client " << this->_client_id << "]" << RESET
		<< GRAY << "\n\t[REQUEST START]\n"
		<< *this->_request
		<< "\t[REQUEST END]" << RESET << std::endl;
	return ;
}

