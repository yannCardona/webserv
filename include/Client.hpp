#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <ctime>
# include <cassert>

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>

# include "Request.hpp"
# include "Response.hpp"

class Client {
public:
	Client();
	Client(int connectedSD, const ServerConfig& responsibleServer);
	Client(const Client &other);
	~Client();

	const int					&getSocket() const;
	const struct sockaddr_in	&getAddress() const;
	Request						&getRequest(); //no const because of Request::feedTobuffer
	Response					&getResponse();
	const time_t				&getLastTime() const;
	const std::string   		&getResponseStr() const;
	const bool					&getStartSending() const;
	const ServerConfig			&getVirtualServer() const;

	//CGI
	const bool					&getCGIStatus() const;
	const int					&getReadPipeFromChild() const;
	const int					&getWritePipeToChild() const;
	const int					&getChildPID() const;

	void						setErrorPage(std::string statusCode, std::string reasonPhrase);
	void						setCGIStatus(bool status);

	void				buildResponse();
	void				updateTime();
	// void				setServer(std::vector<ServerConfig> virtual_server); //function that assigns the correct virtual server to process the request in order to build the response
	void				updateResponseStr(std::string const &remainder);
	void				feedToResponseStr(std::string const &toBeAppended);
	void				resetClient(); //Reset the Response, Request and request_str
private:
	int                 _connectedSD;
	std::time_t			_lastServerInteraction;
	Response			_response;
	Request				_request;
	std::string			_response_str;
	const ServerConfig		_virtualServer;
	
	Client &operator=(const Client &rhs);
} ;

#endif // CLIENT_HPP
