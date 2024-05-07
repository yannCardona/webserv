# include "Client.hpp"
# include "ServerConfig.hpp"

Client::~Client() {}

Client::Client() {}

Client::Client(int connectedSD, const ServerConfig& responsibleServer)
	: _connectedSD(connectedSD), _virtualServer(responsibleServer) {}


/* Copy constructor */
Client::Client(const Client &other) : _virtualServer(other._virtualServer)
{
	if (this != &other)
	{
		_connectedSD = other._connectedSD;
		_lastServerInteraction = other._lastServerInteraction;
		_request = other._request;
		_response = other._response;
		_response_str = other._response_str;
	}
}

//reset response, request and CGI status for new requests from the same client
void	Client::resetClient() {

	Response	newResponse;
	_response = newResponse;
	Request newRequest;
	_request = newRequest;
	_response_str = "";
	_response.setCGIStatus(false);
}

const int     &Client::getSocket() const
{
    return (_connectedSD);
}

Request   &Client::getRequest() {
    return (_request);
}

Response   &Client::getResponse() {
    return (_response);
}

const std::string   &Client::getResponseStr() const {
    return (_response_str);
}

const time_t     &Client::getLastTime() const
{
    return (_lastServerInteraction);
}

const ServerConfig &Client::getVirtualServer() const {
	return (_virtualServer);
}

const bool&	Client::getCGIStatus() const { return _response.getCGIStatus(); }
const int&	Client::getReadPipeFromChild() const { return _response.getReadPipeFromChild(); }
const int&	Client::getWritePipeToChild() const { return _response.getWritePipeToChild(); }
const int&	Client::getChildPID() const { return _response.getChildPID(); }

void		Client::setCGIStatus(bool status) { _response.setCGIStatus(status); }

void		Client::setErrorPage(std::string statusCode, std::string reasonPhrase) {
	_response.setStatusCode(statusCode, reasonPhrase);
	_response_str = _response.formatHTTPResponse();
}

void        Client::buildResponse()
{
    _response_str = _response.buildResponse(_request, _virtualServer);
}

void             Client::updateTime()
{
    _lastServerInteraction = std::time(NULL);
}

void	Client::updateResponseStr(std::string const &remainder) {
	_response_str = remainder;
}

void	Client::feedToResponseStr(std::string const &toBeAppended) {
	_response_str.append(toBeAppended.c_str());
}

// #define DEFAULT_IP "0.0.0.0"
// #define DEFAULT_PORT 80

/**
 * function that assigns the correct virtual server to process the request 
 * in order to build the response
 * 
 * -first look for possible candidates
 * 
 * look for the best candidate:
 * -first look for a server_name that matches exactly the host header (if multiple, first one chosen)
 * -longest match with leading * wildcard in server_name is chosen
 * -longest match with trailing * wildcard in server_name
 * -no match regular expression
 * -no mtach -> select default server block for that ip address and port
 * 
 * i need: ip, host, server_name
*/
// ServerConfig	Client::setServer(std::vector<ServerConfig> virtualServers) {
// 	std::vector<ServerConfig>	candidates;

// 	std::string					host = _request.getHost();
// 	size_t		pos = host.find(":");
// 	std::string ip = host.substr(0, pos);

// 	if (ip.empty()) //i dont think you can even get a request that contains no ip
// 	    ip = DEFAULT_IP;
	
// 	int port = std::atoi(host.substr(pos + 1).c_str());
// 	if (port == 0)
// 	    port = DEFAULT_PORT;

// 	//if port and ip match we can immediately return that virtual Server
// 	//concurrently we build a list of possible candidates that could
// 	//resolve the connection (only processed if not direct match is found)
// 	for (std::vector<ServerConfig>::iterator it = virtualServers.begin();
// 		it != virtualServers.end(); ++it) {
// 			if (it->getIp() == ip && it->getPort() == port)
// 				return (*it);
// 			if (it->getIp() == DEFAULT_IP && it->getPort() == port) //port must match exactly
// 				candidates.push_back(*it);
// 	}
// 	if (candidates.size() == 1)
// 		return (candidates.back());
	
// 	for (std::vector<ServerConfig>::iterator it = candidates.begin();
// 		it != candidates.end(); ++it) {
// 			if (it->getIp() == DEFAULT_IP && it->getPort() == port) //port must match exactly
// 				return (*it);
// 	}

// 	//look for less specific matches

// 	//at this point the only possible match would be a virtual server with the same port
// 	//and the default ip
// 	for (std::vector<ServerConfig>::iterator it = virtualServers.begin();
// 		it != virtualServers.end(); ++it) {
// 			if (it->getIp() == DEFAULT_IP && it->getPort() == port) //port must match exactly
// 				return (*it);
// 	}

// 	LogMsg(RED) << "It should never come that far, somehow we accepted a connection"
// 		<< "to a Client but we lost the respective virtual server information" 
// 		<< ". Or I wrote the setServer function wrong (more likely)";
// 	exit(EXIT_FAILURE);

// }
