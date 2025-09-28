

#pragma once

#include "headers.hpp"
#include "serverSocket.hpp"
#include "serverPoll.hpp"
#include "Client.hpp"

class WebServ {
	public :
	// CONSTRUCTORS
		WebServ();
		WebServ(const WebServ &other);
		WebServ &operator=(const WebServ &other);
	//DESTUCTORS
		~WebServ();
	//GETTERS
	//SETTERS
	//MEMBER FUNCTIONS
		int					WebServInit(std::string config_file);
		int					WebServRun();
		int					newClient();
		int					removeClient(int indx);
		int					setConfig(std::string config_file);
	private :
		Config					_config;
		serverSocket			_server;
		serverPoll				_poll;
		std::vector<Client *>	_clients;
};