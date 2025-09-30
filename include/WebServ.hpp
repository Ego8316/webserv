

#pragma once

#include "headers.hpp"
#include "Client.hpp"
#include "serverSocket.hpp"
#include "serverPoll.hpp"
#include "Cookie.hpp"
#include "Config.hpp"

class WebServ {
	public :
	// CONSTRUCTORS
		WebServ();
		WebServ(const WebServ &other);
		WebServ &operator=(const WebServ &other);
	//DESTUCTORS
		~WebServ();
	//GETTERS
		Client 				*getClient(int uid);
	//SETTERS
	//MEMBER FUNCTIONS
		int					WebServInit(std::string config_file);
		int					WebServRun();
		int					newClient();
		int					removeClient(int indx);
	private :
		Config					_config;
		serverSocket			_server;
		serverPoll				_poll;
		std::vector<Client *>	_clients;
};