# include "Server.hpp"

/*------------------EXCEPTIONS -----------------*/

// Server::ServerException::ServerException(const std::string& message) : _errorMessage(message) {}

// const char *Server::ServerException::what() const throw() {
// 	return (_errorMessage.c_str());
// }

/*------------------PUBLIC METHODS -----------------*/

/**
 * socket:
 * 	create a socket: endpoint for communication
 * 	AF_INET: IPv4 Internet protocols
 * 	SOCK_STREAM: used for TCP/IP. reliable, two-way, connection-based byte streams
 * 
 * setsockopt:
 * 	used to set socket options
 * 	bind multiple port to the same port or to reuse them even if the socket were recently closed
 * 	SOL_SOCKET: manipulate options at the sockets API level
 * 	SO_REUSEADDR: rules used for validating addresses in bind() should allow reuse of local addresses
 * 	-> for AF_INET: socket binds, accept if there is active listening
 * 	-> When the listening socket is bound to INADDR_ANY with a specific port
 * 		then it is not possible to bind to this port for any local address
 * 	SO_REUSEPORT: 
 * 		-> permits multiple AF_INET sockets to be bound to an identical socket address.  
 * 		-> option has to be set on each socket prior to calling bind(2) on the socket.
 * 
 * sockaddr_in:
 * 	sin_family: needs always to be set to AF_INET
 * 	sin_port: port in network byte order
 * 	sin_addr: host IP address. in network byte order as ensured by htons (host to network)
 * 		-> should be assigned to one of the INADDR_* values
 * 			(INADDR_ANY (0.0.0.0) means any address for binding)
 * 
 * bind:
 * 	assigns a name to an unnamed socket. address will be assigned to that socket
 * 
 * listen:
 * 	expresses willingness to accept incoming connections and a queue limit for
 * 	incoming connections
*/

Server::Server(void) {
	FD_ZERO(&this->_RmasterSet); //initialize fdset for select
	FD_ZERO(&this->_WmasterSet);
}

Server::~Server(void)
{}

std::vector<ServerConfig> Server::parse(std::string filename) throw(std::runtime_error)
{
	try
	{
		_serverConfigs = ServerConfig::parseConfigFile(filename);
		_serverConfigs = ServerConfig::completeConfig(DEFAULT_CONFIG, _serverConfigs);
		_serverConfigs = ServerConfig::splitConfig(_serverConfigs);
		if (ServerConfig::checkDouble(_serverConfigs) == 1)
			throw std::runtime_error("Double ports");
		return _serverConfigs;
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error("ERROR: Configfile parsing!");
	}
	return std::vector<ServerConfig>();
}

void	Server::init(std::string filename) throw (std::runtime_error) {
	int	listenSD;
	struct in_addr addr;

	LOG_MSG(LOG_SERVER, BG_MAGENTA, "[" << timeString() << "]   " << "Creating Servers ...  ");
	_serverConfigs = parse(filename);
	for (std::vector<ServerConfig>::iterator it = _serverConfigs.begin();
		it != _serverConfigs.end(); ++it) {
			LOG_MSG(LOG_SERVER, BG_MAGENTA, "[" << timeString() << "]   " << "Initializing Server ...")
			LOG_MSG(LOG_SERVER, BG_MAGENTA, "[" << timeString() << "]   " <<"Hostname/ip read from config:"
			<< it->getHostname() << " Port from config:" << it->getPort());

			inet_pton(AF_INET, it->getHostname().c_str(), &addr);
			char ip_buffer[INET_ADDRSTRLEN];
    		const char *result = inet_ntop(AF_INET, &addr, ip_buffer, INET_ADDRSTRLEN);

			LOG_MSG(LOG_DEBUG, BG_WHITE, "Hostname/ip converted to binary:" << addr.s_addr 
				<< " reconverted to string:" << result);

			listenSD = this->_createSocket(it->getPort(), addr.s_addr);

			_virtualServers.insert(std::make_pair(listenSD, *it));
			LOG_MSG(LOG_SERVER, BG_MAGENTA, "[" << timeString() << "]   " << "Server created | ServerName: "
				<< it->getServerName() << " | HostName: " << it->getHostname() << " | Port: " << it->getPort()
				<< " | SD: " << listenSD);
    }
}

int	Server::_createSocket(in_port_t port, in_addr_t ip) throw (std::runtime_error) {
	struct sockaddr_in	address;
	int					listenSD;
	int 				option = 1;

	//intialize address structure
	std::memset((void *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = ip;

	// address.sin_addr.s_addr = htonl(ip);
	if ((listenSD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw (std::runtime_error("socket creation failed"));
	
	if(fcntl(listenSD, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("fcntl failed"));

	if (setsockopt(listenSD, SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT,
		&option, sizeof(int)) == -1) {
		close(listenSD);
		throw (std::runtime_error("setsockopt failed"));
	}

	if ((bind(listenSD, (struct sockaddr *) &address, sizeof(sockaddr_in))) == -1)
		throw (std::runtime_error("bind failed"));

	if (listen(listenSD, LISTEN_BACKLOG) == -1)
		throw (std::runtime_error("listen failed"));

	//for select
	this->_maxFD = listenSD;
	FD_SET(listenSD, &this->_RmasterSet);
	return (listenSD);
}

/**
 * FD_ISSET: true if fd is part of the set specified
 * 
 * loop through fds until _maxFD and close them if they are contained
 * in the fdset from select
*/
void	Server::exitServer() {
	// for (int i = 0; i <= this->_maxFD; ++i) {
    //   if (FD_ISSET(i, &this->_RmasterSet) || FD_ISSET(i, &this->_WmasterSet)) //check if working set makes more sense
    //     close(i);
	// }
	// for (std::set<int>::iterator it = this->_listenSDs.begin(); it != this->_listenSDs.end(); ++it) {
    //    close(*it);
    // }
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
       close(it->first);
    }
	for (std::map<int, ServerConfig>::iterator it = _virtualServers.begin(); it != _virtualServers.end(); ++it) {
       close(it->first);
    }
	
}

void	Server::_addToSet(int fd, fd_set* set) {
	LOG_MSG(LOG_DEBUG, BG_WHITE, "[" << timeString() << "]   " << "fd " << fd << " added to " << ((set == &_RmasterSet) ? "Readset" : "Writeset"));
	FD_SET(fd, set);
	if (fd > this->_maxFD)
		this->_maxFD = fd;
}

void	Server::_rmFromSet(int fd, fd_set* set) {
	LOG_MSG(LOG_DEBUG, BG_WHITE, "[" << timeString() << "]   " << "fd " << fd << " removed from " << ((set == &_RmasterSet) ? "Readset" : "Writeset"));
	FD_CLR(fd, set);
	// if (fd == this->_maxFD) {
	// 	while (FD_ISSET(this->_maxFD, set) == false && _maxFD >= 0)
	// 		--this->_maxFD;
	// }
}

void	printCR(std::string str) {
	size_t i = 0;
	while (i < str.length()) {
		if (str[i] == '\n')
			std::cout << "\\LF";
		else if (str[i] == '\r')
			std::cout << "\\CR";
		else
			std::cout << str[i];
		i++;
	}
}
