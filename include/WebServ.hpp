

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
		WebServ(Config *config);
		WebServ(std::string config_file);
		WebServ(const WebServ &other);
		WebServ &operator=(const WebServ &other);
	//DESTUCTORS
		~WebServ();
	//GETTERS
		Client 				*getClient(int uid);
	//SETTERS
	//MEMBER FUNCTIONS
		int					WebServInit();
		int					WebServRun();
		int					newClient();
		int					removeClient(int indx);
		int					WebServReboot();
	private :
		Config					*_config;
		serverSocket			*_server;
		serverPoll				*_poll;
		Cookie					*_cookies;
		std::vector<Client *>	_clients;
};