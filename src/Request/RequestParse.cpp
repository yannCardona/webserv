#include "Request.hpp"

int	Request::parseRequest(unsigned long max_body_size) {
	std::string			line;

	//check for client body size limit
	if (_header_parsed == false) {
		if (parseFirstLine() != 0) 
			return (-1);
		if (parseHeader() != 0)
			return (-1);
		_header_parsed = true;
		if (!_request_header.count("Content-Length"))
			_finished = true;
		else
			_body = true;
	}
	if (max_body_size < _buffer.size()) {
		// LOG_MSG(LOG_ERROR, BG_RED, "[" << timeString() << "]   " << "ERROR Request Parsing | Status Code: 413 - Payload Too Large");
		return (_finished = true, _bad_body_size = true, -1);
	}
	if (_body == true && _buffer.size() == _content_length) {
		if (_multipart && parseMultiPart() == -1)
			return (-1);
		else {
			_request_body = _buffer;
			_finished = true;
		}
	}
	return (0);
}

int	Request::parseFirstLine() {
	std::string	request_line;
	int	start;
	size_t	i;

	//get first line
	i = _buffer.find("\r\n");
	if (i == std::string::npos)
		return (_bad_request = true, -1);
	request_line = _buffer.substr(0, i + 2);
	_parsing_counter = request_line.length();
	
	i = start = 0;
	while (request_line[i] && request_line[i] != ' ')
		i++;
	_method = request_line.substr(start, i);
	i++;
	if (!request_line[i] || request_line[i] == ' ')
		return (_bad_request = true, -1);
	start = i;
	while (request_line[i] && request_line[i] != ' ' && request_line[i] != '?')
		i++;
	_request_target = request_line.substr(start, i - start);
	if (request_line[i] == '?') {
		start = i;
		while (request_line[i] && request_line[i])
			i++;
		_query = request_line.substr(start, i - start);
	}
	i++;
	if (!request_line[i] || request_line[i] == ' ')
		return (_bad_request = true, -1);
	start = i;
	while (request_line[i] && request_line[i] != '\r')
		i++;
	_http_version = request_line.substr(start, i - start);
	//shift the buffer
	_buffer = _buffer.substr(_parsing_counter);
	return (0);
}

int	Request::parseHeader() {
	std::string			line;
	std::string			key;
	std::string			value;

	//get the whole header
	size_t	i = _buffer.find("\r\n\r\n");
	if (i == std::string::npos)
		return (_bad_request = true, -1);
	std::string	header_str = _buffer.substr(0, i + 4);

	//parse the header string
	std::stringstream	ss(header_str);
	while (getline(ss, line)) {
		if (line == "\r")
			break;
		std::istringstream line_stream(line);
		getline(line_stream, key, ':');
		if (std::isspace(key[key.length() - 1]))
			return (_bad_request = true, -1);
    	getline(line_stream >> std::ws, value);
		//handle the pending /r
		if (value[value.length() - 1] != '\r') 
			return (_bad_request = true, -1);
		value = value.substr(0, value.length() - 1);
		if (key == "Host") //save host for identifying corresponding virutal server
			_host = value;
		if (key == "Content-Type" && value.find("multipart") != std::string::npos) {
			_multipart_boundary = "--" + value.substr(value.find("=") + 1);
			_multipart = true;
		}
		if (key == "Content-Length")
			_content_length = std::atol(value.c_str());
		if (key == "Connection" && value.find("keep-alive") != std::string::npos)
			_keepAlive = true;
		_request_header.insert(std::make_pair(key, value));
	}
	//shift the buffer
	_buffer = _buffer.substr(i + 4);
	return (0);
}
