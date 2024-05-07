#include "../../include/Response.hpp"

Response::Response() : _response_body(""), _isCGI(false) {}

Response::Response(Response const &src) {
	if (this != &src)
		*this = src;
}

Response::~Response() {}

Response &Response::operator=(Response const &rhs) {
	if (this != &rhs) {
		//_http_version = rhs._http_version;
		_status_code = rhs._status_code;
		_reason_phrase = rhs._reason_phrase;
		_response_header = rhs._response_header;
		_response_body = rhs._response_body;
		_virtual_server = rhs._virtual_server;
		_request_target = rhs._request_target;
		_root = rhs._root;
		_error_page = rhs._error_page;
		_allowed_methods = rhs._allowed_methods;
		_client_body_buffer_size = rhs._client_body_buffer_size;
		_index = rhs._index;
		_location = rhs._location;
	}
	return (*this);
}

const std::string	&Response::getStatusCode() const { return _status_code; }

const bool&	Response::getCGIStatus() const { return _isCGI; }
const int&	Response::getReadPipeFromChild() const { return _PipeFromChild[0]; }
const int&	Response::getWritePipeToChild() const { return _PipeToChild[1]; }
const int&	Response::getChildPID() const { return _childPID; }

void		Response::setCGIStatus(bool status) { _isCGI = status; }

void	Response::setStatusCode(std::string statusCode, std::string reasonPhrase) {
	_status_code = statusCode;
	_reason_phrase = reasonPhrase;
	buildErrorBody();	
}

std::string	Response::formatHTTPResponse() {
	std::ostringstream ss;
	ss << "HTTP/1.1" << ' ' << _status_code << ' ' << _reason_phrase << "\r\n";
	std::map<std::string, std::string>::iterator it;
	for (it = _response_header.begin(); it != _response_header.end(); it++)
		ss << it->first << ": " << it->second << "\r\n";
	ss << "Connection: keep-alive\r\n";
	ss << "\r\n";
	ss << _response_body;
	return ss.str();
}

// 415 (Unsupported Media Type) status code 
std::string	ft_findType(std::string path) {
	std::map<std::string, std::string>	types_map;
	std::map<std::string, std::string>::iterator it;
	
	types_map.insert(std::make_pair(".txt", "text/plain"));
	types_map.insert(std::make_pair(".html", "text/html"));
	types_map.insert(std::make_pair(".css", "text/css"));
	types_map.insert(std::make_pair(".js", "text/javascript"));
	types_map.insert(std::make_pair(".js", "text/javascript"));
	types_map.insert(std::make_pair(".jpeg", "image/jpeg"));
	types_map.insert(std::make_pair(".png", "image/png"));
	types_map.insert(std::make_pair(".gif", "image/gif"));
	types_map.insert(std::make_pair(".mpeg", "audio/mpeg"));
	types_map.insert(std::make_pair(".wav", "audio/wav"));
	types_map.insert(std::make_pair(".mp3", "audio/mp3"));
	types_map.insert(std::make_pair(".mp4", "video/mp4"));
	types_map.insert(std::make_pair(".webm", "video/webm"));
	types_map.insert(std::make_pair(".json", "application/json"));
	types_map.insert(std::make_pair(".pdf", "application/pdf"));
	types_map.insert(std::make_pair(".xml", "application/xml"));

	size_t dot_pos = path.find_last_of('.');
    if (dot_pos != std::string::npos && dot_pos < path.length() - 1) {
        it = types_map.find(path.substr(dot_pos));
		if (it != types_map.end())
			return (it->second);
    }
	return ("");
}

std::string to_string(int const &i) {
    std::ostringstream oss;
    oss << i;
    return oss.str();
}

bool	isDirectory(const char* path) {
	struct stat fileInfo;

	if (stat(path, &fileInfo) != 0)
		return false;
	return S_ISDIR(fileInfo.st_mode);
}

extern char**	environ;

void	Response::buildCGI(Request const &request) {
	(void)request;
	pid_t	pid;
	char* argv[3];
	std::string scriptName = _root + _request_target;

	//remove everything until beginning of script name
	// scriptName = scriptName.substr(scriptName.rfind('/') + 1);
	// LOG_MSG(LOG_DEBUG, BG_WHITE, "script name: " << scriptName);
	
	argv[0] = strdup(CGI_INTERPRETER_PY);
	argv[1] = strdup(scriptName.c_str());
	argv[2] = NULL;
	
	if (pipe(_PipeToChild) == -1 || pipe(_PipeFromChild))
		perror("pipe");
	
	pid = fork();
	if (pid == -1)
		perror("fork");
	if (pid == 0)
	{
		//LOG_MSG()
		if (chdir(_root.c_str()) == -1) {
			perror("cannot change to cgi-bin");
			return;
		}
		//write to write end of pipe instead of STDOUT
		dup2(_PipeFromChild[1], STDOUT_FILENO);
		dup2(_PipeToChild[0], STDIN_FILENO);

		//close unused pipe ends
		close(_PipeFromChild[0]);
		close(_PipeToChild[1]);

		//no longer needed after dup2, can be closed
		close(_PipeFromChild[1]);
		close(_PipeToChild[0]);
		
		if (execve(argv[0], argv, environ) == -1)
			perror("execve");
	}
	free(argv[0]);
	free(argv[1]);

	_childPID = pid;

	close(_PipeFromChild[1]);
	close(_PipeToChild[0]);
	//close(pipefd[1]); //need to be closed later after writing to child
}

bool	Response::checkAllowedMethod(std::string const &method) {
	std::set<std::string>::iterator	it;

	it = _allowed_methods.find(method);
	if (it != _allowed_methods.end())
		return (true);
	else
		return (false);
}

void	Response::printResponse() {
	std::cout << "HTTP/1.1" << ' ' << _status_code << ' ' << _reason_phrase << "\r\n";
	std::map<std::string, std::string>::iterator it;
	for (it = _response_header.begin(); it != _response_header.end(); it++)
		std::cout << it->first << " : " << it->second << "\r\n";
	std::cout << "\r\n" << _response_body;
}