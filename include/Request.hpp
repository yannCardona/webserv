/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbaumann <lbaumann@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 11:37:14 by ycardona          #+#    #+#             */
/*   Updated: 2023/12/21 11:10:12 by lbaumann         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <map>
# include <cstdlib> //atoi
# include <vector>
# include <ctime>

# include "LogMsg.hpp"

class Request {
public:
	Request();
	Request(Request const &src);
	~Request();
	Request &operator=(Request const &rhs);
	
	std::string	getMethod() const;
	std::string	getRequestTarget() const;
	const std::string&	getHost() const;
	std::string	getRequestBody() const;
	bool	getBadRequest() const;
	bool	getBadBodySize() const;
	bool	getKeepAlive() const;

	void	updateRequestBody(std::string const &remainder);
	std::string	getFileName() const;

	void	printBuffer() const;
	void	feedToBuffer(char *buffer, size_t bytesTransmitted);
	bool	headerIsRead();
	bool	isFinished() const;
	int		parseRequest(unsigned long max_body_size);

	friend std::ostream& operator<<(std::ostream &os, Request &obj);
	
private:
	std::string	_method;
	std::string	_request_target;
	std::string	_query;
	std::string	_http_version;
	std::map<std::string, std::string>	_request_header;
	std::string	_request_body;
	std::string	_buffer;
	std::string	_host;
	bool		_bad_request;
	bool		_bad_body_size;
	bool		_body;
	bool		_header_parsed;
	bool		_finished;
	bool		_keepAlive;
	unsigned long int	_content_length;
	long int	_parsing_counter;
	
	//for multipart parsing
	std::vector< std::map<std::string, std::string> >	_multi_headers;
	std::vector<std::string>							_multi_bodys;
	std::string											_multipart_boundary;
	bool												_multipart;
	bool												_first_boundary_parsed;
	
	//cookie and session management
	bool		_cookie;

	int	parseFirstLine();
	int	parseHeader();
	int	parseMultiHeader();
	int	parseMultiPart();
	int	parseMultiBody();

} ;

std::string	timeString();

#endif