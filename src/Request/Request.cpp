/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ycardona <ycardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 07:59:55 by ycardona          #+#    #+#             */
/*   Updated: 2024/01/15 19:24:03 by ycardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() : _bad_request(false), _bad_body_size(false), _body(false),
	_header_parsed(false), _finished(false), _keepAlive(false),
	_multipart(false), _first_boundary_parsed(false), _cookie(false) { }

Request::Request(Request const &src) {
	if (this != &src)
		*this = src; 
 }

Request::~Request() { }

Request &Request::operator=(Request const &rhs) {
	if (this != &rhs)
	{
		_method = rhs._method;
		_request_target = rhs._request_target;
		_query = rhs._query;
		_http_version = rhs._http_version;
		_request_header = rhs._request_header;
		_request_body = rhs._request_body;
		_buffer = rhs._buffer;
		_host = rhs._host;
		_bad_request = rhs._bad_request;
		_bad_body_size = rhs._bad_body_size;
		_body = rhs._body;
		_header_parsed = rhs._header_parsed;
		_finished = rhs._finished;
		_content_length = rhs._content_length;
		_parsing_counter = rhs._parsing_counter;
		_multi_headers = rhs._multi_headers;
		_multi_bodys = rhs._multi_bodys;
		_multipart_boundary = rhs._multipart_boundary;
		_multipart = rhs._multipart;
		_first_boundary_parsed = rhs._first_boundary_parsed;
		_cookie = rhs._cookie;
	}
	return (*this);
}

bool	Request::isFinished() const {
	return (_finished);
}

std::string	Request::getMethod() const {
	return (_method);
}

std::string	Request::getRequestTarget() const {
	return (_request_target);
}

bool	Request::getBadRequest() const {
	return (_bad_request);
}

void	Request::updateRequestBody(std::string const &remainder) {
	_request_body = remainder;
}

bool	Request::getBadBodySize() const {
	return (_bad_body_size);
}

bool	Request::getKeepAlive() const { return _keepAlive; };

//funktion return nur den ersten body wenn es ein multipart body ist
std::string	Request::getRequestBody() const {
	if (_body == false)
		return ("No Body :'-(");
	if (_multipart)
		return (_multi_bodys[0]);
	else
		return (_request_body);
}

std::string	Request::getFileName() const {

	if (_multipart == true)
	{
		std::map<std::string, std::string> header = _multi_headers[0];
		std::string input = header["Content-Disposition"];

		size_t pos = input.find("filename=");

		if (pos != std::string::npos)
			return input.substr(pos + 10, input.size() - (pos + 10 + 1));
		else
			return "file";	
	}
	else
		return "file.txt";
}

void	Request::feedToBuffer(char *buffer, size_t bytesTransmitted) {
	size_t	i;

	i = 0;
	while (i < bytesTransmitted) {
		_buffer += buffer[i];
		i++;
	}
}

bool	Request::headerIsRead() {
	if (_header_parsed)
		return (true);
	size_t found = _buffer.find("\r\n\r\n");
	if (found != std::string::npos)
		return (true);
	else
		return (false);
}

void	Request::printBuffer() const {
	std::cout << _buffer << std::endl;
}

std::ostream& operator<<(std::ostream &os, Request &obj) {
	os << "Method: " << obj._method << std::endl;
	os << "request target: " << obj._request_target << std::endl;
	os << "HTTP version: " << obj._http_version << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = obj._request_header.begin(); it != obj._request_header.end(); it++)
		os << it->first << " : " << it->second <<  std::endl;
	os << std::boolalpha << "bad request: " << obj._bad_request << std::endl;
	os << std::boolalpha << "body: " << obj._body << std::endl;
	if (obj._body == true && obj._multipart == false)
		os << "body: " << obj._request_body << std::endl;
	else if (obj._body == true && obj._multipart == true) {
		os << "body header:" << std::endl;
		std::vector< std::map<std::string, std::string> >::iterator	it_h;
		std::vector<std::string>::iterator	it_b;
		it_b = obj._multi_bodys.begin();
		for (it_h = obj._multi_headers.begin(); it_h != obj._multi_headers.end(); it_h++) {
			for (it = it_h->begin(); it != it_h->end(); it++)
				os << it->first << " : " << it->second <<  std::endl;
			os << "\n" << *it_b << std::endl;
		}
	}
	return (os);
}

std::string	timeString() {
		// Get the current time
		std::time_t currentTime = std::time(NULL);
		// Convert time_t to struct tm
		std::tm* timeInfo = std::localtime(&currentTime);
		// Format the time as a string
		char buffer[80];  // Adjust the size as needed
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
		// Convert the C-style string to a C++ string
		std::string currentTimeString(buffer);
		return (currentTimeString);
}
