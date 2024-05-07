#include "Server.hpp"

/**
 * FOR DEBUGGING
 * prints the fds in a fd_set that select selected to be ready for an I/O manipulation
*/
// static void printSetFileDescriptors(const fd_set *set, int maxFD) {
//     if (set == NULL) {
//         printf("Error: NULL set\n");
//         return;
//     }

//     printf("File Descriptors Set: ");
//     for (int i = 0; i < maxFD; ++i) {
//         if (FD_ISSET(i, set)) {
//             printf("%d ", i);
//         }
//     }
//     printf("\n");
// }

/**
 * FOR DEBUGGING
 * returns true if any fd is set in a fd_set
*/
// static bool FD_IS_ANY_SET(fd_set const *fdset)
// {
//     static fd_set empty;     // initialized to 0 -> empty
//     return memcmp(fdset, &empty, sizeof(fd_set)) != 0;
// }

/**
 * @brief main execution loop of the server
 * @section
 * infinite loop through select to determine fds that are ready
 * for I/O manipulation. In our case: read the request/send the response
 * to a webserver (or a child process in the case of CGI)
*/
void	Server::run() throw (std::runtime_error) {
	int	readyConnections;

	//we have a timeout for select in order to continue the main event
	//loop and check if one of our connected clients should timeout
	struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

	while (g_runServer) {
		//use working set for select to preserve original master set (changed by select)
		std::memcpy(&_RworkingSet, &_RmasterSet, sizeof(_RmasterSet));
		std::memcpy(&_WworkingSet, &_WmasterSet, sizeof(_WmasterSet));

		readyConnections = select(this->_maxFD + 1, &this->_RworkingSet,
			&this->_WworkingSet, NULL, &timeout);

		//only case where our server should stop running
		if (readyConnections == -1) {
			LOG_MSG(LOG_ERROR, BG_RED, "select failed");
			perror("");	
			throw (std::runtime_error("select failed"));
		}
		
		for (int i = 3; i <= _maxFD; i++) {
			if (FD_ISSET(i, &_RworkingSet) && _virtualServers.count(i)) {
				_acceptConnections(i);
			} else if (_clients.count(i)) {
				Client&	client = _clients[i];
				if (client.getCGIStatus() == false && FD_ISSET(i, &_RworkingSet))
					_receiveFromClient(client);
				else if (client.getCGIStatus() == true && FD_ISSET(client.getWritePipeToChild(), &_WworkingSet))
					_sendToCGI(client);
				else if (client.getCGIStatus() == true && FD_ISSET(client.getReadPipeFromChild(), &_RworkingSet))
					_receiveFromCGI(client);
				else if (FD_ISSET(i, &_WworkingSet))
					_sendToClient(client);
			}
		}
		_checkTimeout();
	}
	exitServer();
}

/**
 * @brief accept incoming connections
 * @param listenSD descriptor of socket that was marked by select as "ready"
 * @section
 * -accept is usually a blocking operation. but as all sockets are marked
 * as nonblocking, it does not block.
 * -on success accept returns the file descriptor of the accepted socket
 * -we put the accepted socket into non blocking mode aswell
 * -this fd enables us the communication with the client, it is added to
 * the read set for select
 * -a client is created and assigned the fd + the corresponding virtual server
 * -update the time for the client, to make tracking timeouts possible
 * 
*/
void	Server::_acceptConnections(int listenSD){
	int					connectedSD;

	connectedSD = accept(listenSD, NULL, NULL);
	if (connectedSD == -1) {
		LOG_MSG(LOG_ERROR, BG_RED, "accept failed");
		return ;
	}

	if (fcntl(listenSD, F_SETFL, O_NONBLOCK) == -1) {
		close(connectedSD);
		LOG_MSG(LOG_ERROR, BG_RED, "fcntl failed");
		return ;
	}
	
	LOG_MSG(LOG_CONNECTIONS, BG_CYAN, "[" << timeString() << "]   " << "New Connection from "
		<< _virtualServers[listenSD].getHostname() << " assigned to Socket " << connectedSD);

	Client c(connectedSD, _virtualServers[listenSD]);

	c.updateTime();
	_clients.insert(std::make_pair(connectedSD, c));
	_addToSet(connectedSD, &this->_RmasterSet);
}

/**
 * @brief read request from client, parse it and build the response (client.builtResponse())
 * @param client client that sends the request
 * @section
 * -if receive fails we close the connection to that client and remove the client 
 * from our clients map
 * -in case of CGI, the response is not build (will be build by the CGI script)
*/
void	Server::_receiveFromClient(Client& client){
	int		bytesTransmitted;
	char 	buffer[RECV_BUFFER + 1];
	int		clientSocket; //assign value from client class

	std::memset(buffer, 0, RECV_BUFFER + 1);
	clientSocket = client.getSocket();
	bytesTransmitted = recv(clientSocket, buffer, RECV_BUFFER, 0);
	if (bytesTransmitted == -1) {
		LOG_MSG(LOG_ERROR, BG_RED, "recv error");
		_closeConnection(clientSocket);
		_clients.erase(clientSocket);
		return;
	}
	if (bytesTransmitted == 0) {
		return ;
	}
	client.updateTime();

	client.getRequest().feedToBuffer(buffer, bytesTransmitted);
	
	//checking if Request header was read; then parsing header for body content length
	if (client.getRequest().headerIsRead()) {
		client.getRequest().parseRequest(client.getVirtualServer().getClientBodyBufferSize());
	}

	if (client.getRequest().isFinished()) {
		LOG_MSG(LOG_MESSAGES, BG_GREEN, "[" << timeString() << "]   " << "Request received from Socket " << client.getSocket() << " | Method: " << client.getRequest().getMethod() << " | URI: " << client.getRequest().getRequestTarget());
		LOG_MSG(LOG_REQUEST, BG_YELLOW, client.getRequest());
		client.buildResponse();
		_rmFromSet(clientSocket, &_RmasterSet);
		if (client.getCGIStatus() == true)
			_addToSet(client.getWritePipeToChild(), &_WmasterSet);
		else //if cgi we first need to collect the response from the child 
			_addToSet(clientSocket, &_WmasterSet); //before we can send something back
	}
}

/**
 * @brief send response (created by the webserver) to client
 * @param client client that should receive the reponse
 * @section
 * -if send fails we close the connection to that client and remove the client 
 * from our clients map
*/
void	Server::_sendToClient(Client& client){
	int		bytesTransmitted;
	int		clientSocket= client.getSocket();

	bytesTransmitted = send(clientSocket, client.getResponseStr().c_str(), client.getResponseStr().length(), 0);
	if (bytesTransmitted == -1) {
		LOG_MSG(LOG_ERROR, BG_RED, "send error");
		_closeConnection(clientSocket);
		_clients.erase(clientSocket);
		return;
	}
	if (bytesTransmitted == 0)
		return ;
	client.updateTime();

	//transmission finished, remove from write set, but leave connection open
	//add to readset
	if (static_cast<unsigned long>(bytesTransmitted) == client.getResponseStr().length()) {
		LOG_MSG(LOG_MESSAGES, BG_GREEN, "[" << timeString() << "]   " << "Response sent to Socket "
			<< client.getSocket() << " | Status Code: " << client.getResponse().getStatusCode());
		LOG_MSG(LOG_RESPONSE, BG_BLUE, client.getResponseStr());
		if (client.getRequest().getKeepAlive() == true) {
			_rmFromSet(clientSocket, &_WmasterSet);
			_addToSet(clientSocket, &_RmasterSet);
			client.resetClient();
		} else {
			_closeConnection(clientSocket);
			_clients.erase(clientSocket);
		}
	}
	//if the whole repsone wasnt sent, save the remainder to be sent in next iteration
	else
		client.updateResponseStr(client.getResponseStr().substr(bytesTransmitted));
}

/**
 * @brief go through the client map and disconnect + delete all clients with
 * their last Server interaction in the past longer than TIMEOUT_CNNCT_S seconds
 * @section
 * std::time returns time in seconds
*/
void	Server::_checkTimeout(){
	std::time_t	currentTime = std::time(NULL);
	std::time_t	lastServerInteraction;
	double		delta;

	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end();) {
		lastServerInteraction = it->second.getLastTime();
		delta = std::difftime(currentTime, lastServerInteraction);
		if (delta > TIMEOUT_CNNCT_S) {
			if (it->second.getCGIStatus()) {
				LOG_MSG(LOG_CONNECTIONS, BG_CYAN, "[" << timeString() << "]   " << "Client " << it->first << " CGI TImeout");
				_closeConnection(it->second.getReadPipeFromChild());
				_closeConnection(it->second.getWritePipeToChild());
				_addToSet(it->second.getSocket(), &_WmasterSet);
				it->second.setErrorPage("500", "Internal Server Error");
				it->second.setCGIStatus(false);
				kill(it->second.getChildPID(), SIGKILL);
				it->second.updateTime();
			}
			else {
				LOG_MSG(LOG_CONNECTIONS, BG_CYAN, "[" << timeString() << "]   " << "Client " << it->first << " TIMED OUT, closing!");
				_closeConnection(it->first);
				_clients.erase(it++);
			}
			//remove from active clients
		}
		else
			++it;
	}
}

/**
 * @brief remove fd from read/write fd_set and close it
 * @param fd close connection with fd
*/
void	Server::_closeConnection(int fd){
	if (FD_ISSET(fd, &this->_RmasterSet)) {
		this->_rmFromSet(fd, &this->_RmasterSet);
	}
	if (FD_ISSET(fd, &this->_WmasterSet)) {
		this->_rmFromSet(fd, &this->_WmasterSet);
	}	
	close(fd);
	LOG_MSG(LOG_CONNECTIONS, BG_CYAN, "[" << timeString() << "]   " << "Connection with fd " << fd << " closed");
}
