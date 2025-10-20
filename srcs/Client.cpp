/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ego <ego@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 17:16:23 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/20 19:38:04 by ego              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(Config *config, ServerCore *server)
	:	_config(config),
		_server(server)
{
	this->_client_len = sizeof(this->_client_addr);
	this->_state = TRY_ACCEPTING;
	this->_request = new Request();
	this->_response = Response();
}

Client::Client(const Client &other)
	:	_config(other._config),
		_server(other._server)
{
	*this = other;
}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		this->_client_addr = other._client_addr;
		this->_client_len = other._client_len;
		this->_config = other._config;
		this->_state = other._state;
		if (this->_request)
			delete this->_request;
		this->_request = other._request;
		this->_response = other._response;
	}
	return (*this);
}

Client::~Client()
{
	if (this->_client_fd)
		close(this->_client_fd);
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

int		Client::handleEvent()
{
	int					bytes_read;
	std::string			request_str;
	std::string			response_str;
	std::vector<char>	buffer(_config->buffer_size);

	_time_limit = utils::getTime() + this->_config->processing_time_limit;
	while (utils::getTime() < _time_limit)
	{
		if (_state == TRY_ACCEPTING)
			_tryAccepting();
		if (_state == ABORTING)
			return (SERV_ERROR);
		if (_state == HEADER_READING && _readInput() == SERV_ERROR)
			return (SERV_ERROR);
		if (_state == PROCESSING_REQUEST)
			_processRequest();
		if (_state == CGI_INIT)
			return (0);		// TODO
		if (_state == CGI_WAITING)
			return (0);		// TODO 
		if (_state == OUTPUT_SENDING && _sendOutput() == SERV_ERROR)
			return (SERV_ERROR);
	}
	bytes_read = _config->buffer_size;
	while (bytes_read == _config->buffer_size && request_str.size() < this->_config->max_body_size)
	{
		bytes_read = _server->socketRead(&buffer[0], buffer.size(), this);
		if (bytes_read == SERV_ERROR)
			return (SERV_ERROR);
		request_str += std::string(buffer.begin(), buffer.end()).substr(0, bytes_read);
	}
	if (request_str.length() == 0)
	{
		std::cerr << "Empty request. Ignoring..." << std::endl;
		return (SERV_ERROR);
	}
	this->_state = DONE;
	if (_server->socketWrite(response_str.c_str(), response_str.length(), this) == SERV_ERROR)
		return (SERV_ERROR);
	return (0); //TODO return err code
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
	this->_state = HEADER_READING;
	this->_server->pollAdd(this->getFd(), POLLIN | POLLOUT, this->_client_id);
	std::cout << BLUE << "Accepted client " << this->_client_id << RESET << std::endl;
	return (0);
}

int	Client::_readInput()
{
	return (0);
}

void	Client::_processRequest()
{	
	// std::cout << request << std::endl;
	const Cookie		cookies = _request->getQueryCookies();
	_response = RequestHandler::handle(*_request, *_config, cookies);
	// _state = _response.isCGI() ? CGI_INIT : OUTPUT_SENDING;
	_state = OUTPUT_SENDING;
	return ;
}

int	Client::_sendOutput()
{
	return (0);
}
