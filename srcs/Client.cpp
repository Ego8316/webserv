/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/23 12:35:54 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(Config *config, ServerCore *server)
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
	this->_time_limit = 0;
}

Client::Client(const Client &other)
{
	*this = other;
}

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
		this->_leftover = other._leftover;
		this->_bytes_sent = other._bytes_sent;
		this->_bytes_in_buffer = other._bytes_in_buffer;
		this->_time_limit = other._time_limit;
		if (this->_request)
			delete this->_request;
		this->_request = other._request ? new Request(*other._request) : NULL;
		if (this->_response)
			delete this->_response;
		this->_response = other._response ? new Response(*other._response) : NULL;
	}
	return (*this);
}

Client::~Client()
{
	if (this->_client_fd)
		close(this->_client_fd);
	if (this->_request)
		delete _request;
	if (this->_response)
		delete _response;
}

struct sockaddr_in	&Client::getClientAddr()
{
	return (this->_client_addr);
}

socklen_t	&Client::getClientLen()
{
	return (this->_client_len);
}

int	Client::getId()
{
	return (this->_client_id);
}

int	Client::getFd()
{
	return (this->_client_fd);
}

RequestStage	Client::getState()
{
	return (this->_state);
}

ServerCore	&Client::getServer()
{
	return (*this->_server);
}

long	Client::getTimeLimit()
{
	return (this->_time_limit);
}

void	Client::setClientId(int id)
{
	this->_client_id = id;
}

void	Client::setFd(int fd)
{
	this->_client_fd = fd;
}

void	Client::setState(RequestStage state)
{
	this->_state = state;
}

int	Client::handleEvent()
{
	if (_state == INIT)
	{
		this->_request = new Request();
		this->_response = new Response();
		this->_request_time_limit = utils::getTime() + this->_config->max_request_time;
		_state = HEADER_READING;
	}
	_error = ERR_NONE;
	this->_time_limit = std::min(utils::getTime() + this->_config->processing_time_limit, this->_request_time_limit);
	while (utils::getTime() < this->_time_limit && _state != DONE)
	{
		if (_state == TRY_ACCEPTING)
			_tryAccepting();
		if (_state == ABORTING)
			return (SERV_ERROR);
		if (this->_state == HEADER_READING && this->_readHeader() == SERV_ERROR)
			return (SERV_ERROR);
		if (this->_state == BODY_READING && this->_readBody() == SERV_ERROR)
			return (SERV_ERROR);
		if (_state == PROCESSING_REQUEST)
			_processRequest();
		if (_state == CGI_RUNNING)
			_monitorCGI();
		if (this->_state == SENDING_STRING && this->_sendString() == SERV_ERROR)
			return (SERV_ERROR);
		if (this->_state == SENDING_FILE && this->_sendFile() == SERV_ERROR)
			return (SERV_ERROR);
	}
	if (this->_request_time_limit <= utils::getTime())
		_error = KILL_REQUEST;
	if (_state == DONE || _error > WOULD_BLOCK)
	{
		delete this->_request;
		this->_request = NULL;
		delete this->_response;
		this->_response = NULL;
	}
	return (_error);
}

int	Client::_tryAccepting()
{
	if (this->_server->socketAcceptClient(this) == SERV_ERROR)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return (0);
		this->_state = ABORTING;
		return (SERV_ERROR);
	}
	if (this->_server->pollAvailFor(this->_client_id, POLLIN))
		this->_state = HEADER_READING;
	else
		this->_state = DONE;
	this->_server->pollAdd(this->getFd(), POLLIN | POLLOUT, this->_client_id);
	std::cout << BLUE << "Accepted client " << this->_client_id << RESET << std::endl;
	this->printState();
	return (0);
}

int	Client::_readHeader()
{
	std::vector<char>	buffer(this->_config->buffer_size);
	std::string			&header_str = this->_request->getRawHeader();
	size_t				pos;

	if (!this->_leftover.empty())
	{
		std::cout << YELLOW << "[_readHeader] Applying leftover (" << _leftover.size() << " bytes)" << RESET << std::endl;
		header_str.append(this->_leftover);
		this->_leftover.clear();
	}
	ssize_t	bytes_read = this->_server->socketRead(&buffer[0], buffer.size(), this);
	if (bytes_read == SERV_ERROR)
		return (SERV_ERROR);
	if (bytes_read == WBLOCK)
		return (0);
	if (bytes_read == 0)
	{
		std::cout << CYAN << "[_readHeader] Client closed the connection" << RESET << std::endl;
		return (this->_state = ABORTING, 0);
	}
	std::cout << GREEN << "[_readHeader] Read " << bytes_read << " bytes" << RESET << std::endl;
	header_str.append(buffer.begin(), buffer.begin() + bytes_read);
	if (header_str.size() > this->_config->max_header_size)
		return (this->_state = ABORTING, SERV_ERROR);
	if ((pos = header_str.find("\r\n\r\n")) == std::string::npos)
	{
		std::cout << CYAN << "[_readHeader] Incomplete header — waiting for more" << RESET << std::endl;
		return (0);
	}
	this->_leftover = header_str.substr(pos + 4);
	header_str.erase(pos);
	std::cout << CYAN << "[_readHeader] Header fully received (" << header_str.size() << " bytes)" << RESET << std::endl;
	this->printHeader();
	this->_request->parseHeader(*this->_config);
	if (!this->_request->isChunked() && this->_request->getContentLength() == 0)
	{
		this->_state = PROCESSING_REQUEST;
		printState();
		return (0);
	}
	this->_state = BODY_READING;
	printState();
	return (0);
}

int	Client::_readBody()
{
	std::vector<char>	buffer(this->_config->buffer_size);
	std::string			&body_str = this->_request->getRawBody();
	size_t				pos;

	if (body_str.empty() && !this->_request->isChunked())
		body_str.reserve(this->_request->getContentLength());
	if (!this->_leftover.empty())
	{
		std::cout << YELLOW << "[_readBody] Applying leftover (" << _leftover.size() << " bytes)" << RESET << std::endl;
		body_str.append(this->_leftover);
		this->_leftover.clear();
	}
	ssize_t	bytes_read = this->_server->socketRead(&buffer[0], buffer.size(), this);
	if (bytes_read == SERV_ERROR)
		return (SERV_ERROR);
	if (bytes_read == WBLOCK)
		return (0);
	if (bytes_read == 0)
	{
		std::cout << CYAN << "[_readHeader] Client closed the connection" << RESET << std::endl;
		return (this->_state = ABORTING, 0);
	}
	body_str.append(buffer.begin(), buffer.begin() + bytes_read);
	std::cout << GREEN << "[_readBody] Read " << bytes_read << " bytes (total: " << body_str.size() << ")" << RESET << std::endl;
	if (body_str.size() >= this->_request->getContentLength())
	{
		_leftover = body_str.substr(this->_request->getContentLength());
		body_str.erase(this->_request->getContentLength());
		std::cout << CYAN << "[_readBody] Body complete (" << body_str.size() << " bytes)" << RESET << std::endl;
		this->_state = PROCESSING_REQUEST;
		printState();
		return (0);
	}
	if (this->_request->isChunked() && (pos = body_str.find(NULL_CHUNK)) != std::string::npos)
	{
		_leftover = body_str.substr(pos);
		body_str.erase(pos);
		std::cout << CYAN << "[_readBody] Chunked body complete" << RESET << std::endl;
		this->_state = PROCESSING_REQUEST;
		printState();
		return (0);
	}
	std::cout << CYAN << "[_readBody] Partial body received — waiting for more" << RESET << std::endl;
	return (0);
}

void	Client::_processRequest()
{
	const Cookie		cookies = this->_request->getQueryCookies();
	*this->_response = RequestHandler::handle(*_request, *_config, cookies);
	std::cout << BOLD_BLUE << "[Client " <<  this->_client_id << "]" << RESET
		<< BLUE << " Processing request" << RESET << std::endl;
	this->printRequest();
	if (_response->isCGI())
	{
		this->_state = CGI_RUNNING;
		printState();
	}
	else
	{
		this->_state = SENDING_STRING;
		printState();
	}
	return ;
}

int	Client::_monitorCGI()
{
	if (!_response->getCGI() || _response->getCGI()->isComplete())
	{
		_state = SENDING_STRING;
		return (0);
	}
	_response->getCGI()->Run(*this, *_request, *_config, *_response);
	if (_response->getCGI()->isComplete())
		_state = SENDING_STRING;
	return (0);
}

int	Client::_sendString()
{
	const std::string	&response_str = this->_response->getString();
	int					bytes_to_send = response_str.size() - this->_bytes_sent;

	if (bytes_to_send > 0)
	{
		int	sent = this->_server->socketWrite(response_str.c_str() + this->_bytes_sent, bytes_to_send, this);
		if (sent == SERV_ERROR)
			return (SERV_ERROR);
		if (sent == WBLOCK)
			return (0);
		if (sent == 0)
		{
			std::cout << CYAN << "[_sendString] Client closed the connection" << RESET << std::endl;
			return (this->_state = ABORTING, 0);
		}
		this->_bytes_sent += sent;
		return (0);
	}
	if (this->_response->getFd() > -1)
		this->_state = SENDING_FILE;
	else
		this->_state = DONE;
	this->printState();
	return (0);
}

int	Client::_sendFile()
{
	std::vector<char>	buffer(this->_config->buffer_size);

	ssize_t	bytes_read = read(this->_response->getFd(), &buffer[0], this->_config->buffer_size);
	if (bytes_read < 0)
		return (SERV_ERROR);
	else if (bytes_read == 0)
	{
		close(this->_response->getFd());
		this->_response->setFd(-1);
		this->_state = DONE;
		this->printState();
		return (0);
	}

	ssize_t	bytes_sent = this->_server->socketWrite(&buffer[0], bytes_read, this);
	if (bytes_sent == SERV_ERROR)
		return (SERV_ERROR);
	if (bytes_sent == WBLOCK)
		return (0);
	if (bytes_sent == 0)
	{
		std::cout << CYAN << "[_sendString] Client closed the connection" << RESET << std::endl;
		return (this->_state = ABORTING, 0);
	}
	return (0);
}

void	Client::_prepareNew()
{
	delete this->_request;
	delete this->_response;
	this->_request = new Request();
	this->_response = new Response();
}

void	Client::printState() const
{
	std::cout << BOLD_BLUE << "[Client " <<  this->_client_id << "]" << RESET
		<< BLUE << " State is now " << utils::stateToStr(this->_state) << RESET << std::endl;
	return ;
}

void	Client::printHeader() const
{
	std::cout << BOLD_GRAY << "[Client " << this->_client_id << "]" << RESET
		<< GRAY << "\n\t[HEADER START]\n"
		<< this->_request->getRawHeader()
		<< "\t[HEADER END]" << RESET << std::endl;
	return ;
}

void	Client::printRequest() const
{
	std::cout << BOLD_GRAY << "[Client " << this->_client_id << "]" << RESET
		<< GRAY << "\n\t[REQUEST START]\n"
		<< *this->_request
		<< "\t[REQUEST END]" << RESET << std::endl;
	return ;
}
