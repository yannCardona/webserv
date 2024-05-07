#include "Response.hpp"

std::string	Response::buildResponse(Request const &request, ServerConfig const &virtual_server) {
	std::string method;
	std::ostringstream ss;

	_virtual_server = virtual_server;
	_request_target = request.getRequestTarget();

	//add "/" if request_target is not a file 
	if (_request_target.find(".") == std::string::npos
		&& _request_target[_request_target.length() - 1] != '/')
		_request_target += "/";
	//put configs from ServerConfig
	setConfigs();
	if (!_virtual_server.getLocations().empty()) {
		//select the location most matching to request_target and in there overwrite Configs from location
		selectLocation();
	}
	//build response according to method from request
	method = request.getMethod();
	if (request.getBadRequest()) {
		setStatusCode("400", "Bad Request");
	}
	else if (request.getBadBodySize()) {
		setStatusCode("413", "Payload Too Large");
	}
	else if (!checkAllowedMethod(method)) {
		setStatusCode("405", "Method Not Allowed");
	}
	
	//checks if request target has the corresponding cgi file extension
	else if (_request_target.find(CGI_EXTENSIONS_PY,
		_request_target.length() - std::strlen(CGI_EXTENSIONS_PY)) != std::string::npos) {
		LOG_MSG(LOG_DEBUG, BG_WHITE, "Python script found: initiating CGI");
		_isCGI = true;
		buildCGI(request);
		return std::string();
    }

	else if (_request_target.find(CGI_EXTENSIONS_PHP,
		_request_target.length() - std::strlen(CGI_EXTENSIONS_PHP)) != std::string::npos) {
		LOG_MSG(LOG_DEBUG, BG_WHITE, "PHP script found: initiating CGI");
        _isCGI = true;
		buildCGI(request);
		return std::string();
    }

	else if (method == "GET")
		buildGet();
	else if (method == "POST")
		buildPost(request);
	else if (method == "DELETE")
		buildDelete();
	else {
		setStatusCode("405", "Method Not Allowed");
	}
	if (_status_code != "400" && _status_code != "413" && _status_code != "405")
		buildErrorBody();

	return (formatHTTPResponse());
}

void	Response::buildGet() {
	std::string	path;
	
	path = _root + _request_target;

	//handling the autoindex and index here
	// if target is directory, look for a index file and return it
	// if no index is found make autoindex
	if (_request_target[_request_target.length() - 1] == '/') {
		struct stat fileStat;
		if (stat(path.c_str(), &fileStat) != 0) {
			if (S_ISDIR(fileStat.st_mode) == false) {
				_status_code = "404";
				return;
			}
		}
		if (stat((path + _index).c_str(), &fileStat) == 0)
			path += _index;
		else if (_autoindex == true) {
			buildAutoIndex();
			return;
		}
		else {
			_status_code = "403";
			_reason_phrase = "Forbidden";
			return;
		}
	}
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		std::string fileContents(
			(std::istreambuf_iterator<char>(file)),
			(std::istreambuf_iterator<char>()));
		_response_body = fileContents;
		_status_code = "200";
		_reason_phrase = "OK";
		_response_header.insert(std::make_pair("Content-Type", ft_findType(path)));
		_response_header.insert(std::make_pair("Content-Length", to_string(_response_body.length())));
		file.close();
	}
	else {
		_status_code = "404";
		_reason_phrase = "Not Found";
	}
}

void	Response::buildAutoIndex() {
	std::string	directory_path;
	std::map<std::string, std::string> links;

	directory_path = _root + _request_target;

	DIR *dir = opendir(directory_path.c_str());
	if (dir == NULL)
	{
		LOG_MSG(LOG_ERROR, BG_RED, "[" << timeString() << "]   " << "ERROR autoIndex | Status Code: 404 - could not open directory")
		_status_code = "404";
		_reason_phrase = "Not Found";
		return;
	}
	//start of the html file
	_response_body = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index " + _request_target + "</title>\n</head>\n<body>\n\
	<h1>Index of " + _request_target + "</h1>\n<hr>\n";
	//get all the directory entries and generate html-links
	for (struct dirent *directoryEntry = readdir(dir); directoryEntry; directoryEntry = readdir(dir)) {
		//adding a / at the end if its a directory
		std::string dir_ending = "";
		if (directoryEntry->d_type == DT_DIR)
			dir_ending = "/";
		std::string link = "<p><a href=\"" + _request_target + directoryEntry->d_name + dir_ending + "\">" + directoryEntry->d_name + dir_ending + "</a></p>\n";
		links.insert(std::make_pair(directoryEntry->d_name, link));
	}
	_response_body += links[".."];
	links.erase("..");
	links.erase(".");
	std::map<std::string, std::string>::iterator	it;
	for (it = links.begin(); it != links.end(); it++)
		_response_body += it->second;
	//end of the html file
	_response_body += "<hr>\n</body>\n</html>";
	closedir(dir);
	_status_code = "200";
	_reason_phrase = "OK";
	_response_header.insert(std::make_pair("Content-Type", "text/html"));
	_response_header.insert(std::make_pair("Content-Length", to_string(_response_body.length())));
}

void	Response::buildPost(Request const &request) {
	std::string	path;
	
	//creating / updating cookie for favMovie
	if (_request_target == "/html/favMovie.html") {
		_response_header.insert(std::make_pair("Set-Cookie", request.getRequestBody()));
		_status_code = "201";
		_reason_phrase = "Created";
		return ;
	}
	path = _root + _request_target + "/upload_" + request.getFileName();
	// Create an ofstream object for writing to a file
    std::ofstream outputFile(path.c_str());
    // Check if the file is successfully opened
    if (outputFile.is_open()) {
        outputFile << request.getRequestBody();
        outputFile.close();
		_status_code = "201";
		_reason_phrase = "Created";
	}
	else {
		_status_code = "404";
		_reason_phrase = "Not Found";
	}
}

void	Response::buildDelete() {
	std::string	path;
	
	path = _root + _request_target;

	if (std::remove(path.c_str()) == 0) {
		_status_code = "204";
		_reason_phrase = "No Content";
    } else {
		_status_code = "404";
		_reason_phrase = "Not Found";		
    }
}

void	Response::buildErrorBody() {
	//check if status code is error
	std::map<std::string, std::string>::iterator it;
	it = _error_page.find(_status_code);
	if (it == _error_page.end())
		return ;
	LOG_MSG(LOG_ERROR, BG_RED, "[" << timeString() << "]   " << "ERROR | Status Code: " << _status_code << " - " << _reason_phrase);
	std::string	path = _root + _error_page[_status_code];
	std::ifstream file(path.c_str());
	if (file.is_open())
	{
		std::string fileContents(
			(std::istreambuf_iterator<char>(file)),
			(std::istreambuf_iterator<char>()));
		_response_body = fileContents;
		_response_header.insert(std::make_pair("Content-Type", "text/html"));
		_response_header.insert(std::make_pair("Content-Length", to_string(_response_body.length())));
		file.close();
	}
	else {
		path = _error_page[_status_code];
		std::ifstream file(path.c_str());
		if (file.is_open())
		{
			std::string fileContents(
				(std::istreambuf_iterator<char>(file)),
				(std::istreambuf_iterator<char>()));
			_response_body = fileContents;
			_response_header.insert(std::make_pair("Content-Type", "text/html"));
			_response_header.insert(std::make_pair("Content-Length", to_string(_response_body.length())));
			file.close();
		}
	}
}
