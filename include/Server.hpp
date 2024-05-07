#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <stdexcept>
# include <cstring>
# include <set>
# include <memory>
# include <algorithm>
# include <vector>
# include <map>
# include <ctime>

# include <sys/socket.h> //socket
# include <sys/types.h>
# include <sys/wait.h> //waitpid
# include <arpa/inet.h> //htonl, htons
# include <unistd.h> //close
# include <fcntl.h> //fcntl
# include <stdbool.h>
# include <errno.h> //errno
# include <signal.h>
# include <stdlib.h>
# include <sys/time.h> //timeval struct for select

# include "LogMsg.hpp"
# include "ServerConfig.hpp"
# include "Client.hpp"

# define LISTEN_BACKLOG 512
# define DEFAULT_CONFIG "./default_error_pages/default.conf"
# define TIMEOUT_CNNCT_S 60
# define RECV_BUFFER 2000

extern volatile bool g_runServer;

class Server
{
public:
	// public methods
	void	init(std::string filename) throw (std::runtime_error);
	void	run() throw (std::runtime_error);
	void	exitServer();

	// Constructors, Assignment, Destructors
	Server(void);
	~Server(void);


private:
	Server(const Server& copy);
	Server& operator=(const Server& other);
	
	void						_addToSet(int fd, fd_set* set);
	void						_rmFromSet(int fd, fd_set* set);
	std::vector<ServerConfig> 	parse(std::string filename) throw (std::runtime_error);

	//managing the server
	void	_acceptConnections(int listenSD);
	void	_receiveFromClient(Client& c);
	void	_sendToClient(Client& c);
	int		_createSocket(in_port_t port, in_addr_t ip) throw (std::runtime_error);
	void	_checkTimeout();
	void	_closeConnection(int fd);

	void	_sendToCGI(Client& c);
	void	_receiveFromCGI(Client& client);

	// Data
	int							_maxFD; //reassigned with each socket created -> limits the max amount we need to loop
	fd_set						_RworkingSet, _RmasterSet; 	//fds that need to be checked with select (readset)
	fd_set						_WworkingSet, _WmasterSet; 	//fds that need to be checked with select (writeset)
	std::map<int, Client>		_clients;
	std::map<int, ServerConfig> _virtualServers;
	std::vector<ServerConfig>	_serverConfigs;
};

void	printCR(std::string str);

#endif