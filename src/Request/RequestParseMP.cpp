#include "Request.hpp"

int	Request::parseMultiPart()
{
	size_t	i;

	//read and count first boundary and shift buffer
	if (_first_boundary_parsed == false) {
		i = _buffer.find(_multipart_boundary + "\r\n");
		if (i == std::string::npos)
			return (_bad_request = true, -1);
		_parsing_counter = _multipart_boundary.length() + 2;
		_buffer = _buffer.substr(_multipart_boundary.length() + 2);
		_first_boundary_parsed = true;
	}
	while (_finished == false) {
		i = _buffer.find(_multipart_boundary);
		if (i == std::string::npos)
			return (_bad_request = true, -1);
		if (parseMultiHeader() == -1)
			return (-1);
		if (parseMultiBody() == -1)
			return (-1);
	}
	return (0);
}

int	Request::parseMultiHeader()
{
	std::string			line;
	std::string			key;
	std::string			value;
	std::map<std::string, std::string>	header;

	//get and count the whole header
	size_t	i = _buffer.find("\r\n\r\n");
	if (i == std::string::npos)
		return (_bad_request = true, -1);
	std::string	header_str = _buffer.substr(0, i + 4);
	_parsing_counter += header_str.length();

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
		header.insert(std::make_pair(key, value));
	}
	//shift the buffer
	_multi_headers. push_back(header);
	_buffer = _buffer.substr(i + 4);
	return (0);
}

int	Request::parseMultiBody()
{
	size_t	i = _buffer.find(_multipart_boundary);
	if (i == std::string::npos)
		return (_bad_request = true, -1);
	//check the boundary ending 
	std::string	boundary_ending = _buffer.substr(i + _multipart_boundary.length(), 2);
	if (boundary_ending != "\r\n" && boundary_ending != "--")
		return (_bad_request = true, -1);
	//get and count the whole body
	std::string	body_str = _buffer.substr(0, i);
	_parsing_counter += body_str.length() + _multipart_boundary.length() + 2;
	//parse the body string
	if (body_str[body_str.length() - 2] != '\r' || body_str[body_str.length() - 1] != '\n') 
			return (_bad_request = true, -1);
	_multi_bodys.push_back(body_str.substr(0, body_str.length() - 2)); //adding the content to the vector removing the pending /r/n
	_buffer = _buffer.substr(i + _multipart_boundary.length() + 2);
	if (boundary_ending == "--") {
		_finished = true;
		_parsing_counter += 2;
	}
	return (0);
}
