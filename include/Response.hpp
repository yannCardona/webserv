#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <fstream>
# include <ctime>
# include <string>
# include <cstring>
# include <map>
# include <cstdio>

# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <string.h>
# include <dirent.h>

# include "Request.hpp"
# include "ServerConfig.hpp"

#define CGI_EXTENSIONS_PY ".py"
#define CGI_EXTENSIONS_PHP ".php"
#define CGI_INTERPRETER_PY "/usr/bin/python3"
#define CGI_INTERPRETER_PHP "/usr/bin/php"

class Request;

class Response {
private:
	std::string		_status_code;
	std::string		_reason_phrase;
	std::multimap<std::string, std::string>	_response_header; //server, content-type, content-length
	std::string		_response_body;
	ServerConfig	_virtual_server;
	std::string		_request_target;

	//configs
	std::string							_root;
	std::map<std::string, std::string>	_error_page;
	std::set<std::string>				_allowed_methods;
	unsigned long						_client_body_buffer_size;
	std::string							_index;
	Location							_location;
	bool								_autoindex;
	std::map<std::string, std::string>	_cgi_param;
	std::string							_cgi_pass;

	//CGI
	bool	_isCGI; //flag that should be initialized with false in Response creation
	//and switched to true if the server recognizes a cgi request. this flag makes
	//sure that the CGI response is build
	//int		_ReadPipeToChild; //save the fd to the child process that
	//executes the script in order to go through select every time
	//and read the data the child sends back
	int		_PipeToChild[2]; //save the fd to the child process that
	//executes the script in order to go through select every time
	//and read the data the child sends back
	int		_PipeFromChild[2];
	pid_t	_childPID;

	//ResponseConfig.cpp
	void	selectLocation();
	void	setConfigs();
	void	setConfigsLocation();

	//ResponseBuild.cpp
	void		buildGet();
	void		buildPost(Request const &request);
	void		buildDelete();
	void		buildAutoIndex();
	void		buildErrorBody();

	//Response.cpp
	bool	checkAllowedMethod(std::string const &method);
	void	buildCGI(Request const &request);

public:
	Response();
	Response(Response const &src);
	~Response();

	Response &operator=(Response const &rhs);

	const std::string	&getStatusCode() const;

	//ResponseConfig.cpp
	std::string	buildResponse(Request const &request, ServerConfig const &virtual_server);

	//CGI getter
	const bool					&getCGIStatus() const;
	const int					&getReadPipeFromChild() const;
	const int					&getWritePipeToChild() const;
	const int					&getChildPID() const;

	void						setCGIStatus(bool status);

	void	setStatusCode(std::string statusCode, std::string reasonPhrase);
	std::string	formatHTTPResponse();

	void	printResponse();
} ;

std::string to_string(int const &i);
std::string	ft_findType(std::string path);
bool		isDirectory(const char* path);

#endif