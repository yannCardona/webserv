/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elias <elias@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 08:19:58 by elias             #+#    #+#             */
/*   Updated: 2024/01/15 13:25:21 by elias            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	_server_name = "";
	_ports = std::vector<int>();
	_hostnames = std::vector<std::string>();
	_root = "";
	_error_page = std::map<std::string, std::string>();
	_client_body_buffer_size = 0;
	_cgi_param = std::map<std::string, std::string>();
	_cgi_pass = "";
	_location_path = std::vector<std::string>();
	_locations = std::map<std::string, Location>();
	_allowed_methods = std::set<std::string>();
	_index = "";
	_autoindex = false;
}

ServerConfig::~ServerConfig()
{
}

ServerConfig::ServerConfig(const ServerConfig& other)
{
	*this = other;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& other)
{
	if (this != &other)
	{
		_server_name = other._server_name;
		_hostnames = other._hostnames;
		_ports = other._ports;
		_root = other._root;
		_error_page = other._error_page;
		_client_body_buffer_size = other._client_body_buffer_size;
		_cgi_param = other._cgi_param;
		_cgi_pass = other._cgi_pass;
		_location_path = other._location_path;
		_locations = other._locations;
		_allowed_methods = other._allowed_methods;
		_index = other._index;
		_autoindex = other._autoindex;
	}
	return *this;
}

// Getters
const std::string	&ServerConfig::getServerName() const
{
	return _server_name;
}

const int	&ServerConfig::getPort() const
{
	return _ports[0];
}

const std::vector<int>	&ServerConfig::getPorts() const
{
	return _ports;
}

const std::vector<std::string>	&ServerConfig::getHostnames() const
{
	return _hostnames;
}

const std::string	&ServerConfig::getHostname() const
{
	return _hostnames[0];
}

const std::string	&ServerConfig::getRoot() const
{
	return _root;
}

const std::map<std::string, std::string> &ServerConfig::getErrorPage() const
{
	return _error_page;
}

const unsigned long &ServerConfig::getClientBodyBufferSize() const
{
	return _client_body_buffer_size;
}

const std::map<std::string, std::string> &ServerConfig::getCgiParam() const
{
	return _cgi_param;
}

const std::string &ServerConfig::getCgiPass() const
{
	return _cgi_pass;
}

const std::vector<std::string> &ServerConfig::getLocationPath() const
{
	return _location_path;
}

std::map<std::string, Location> &ServerConfig::getLocations()
{
	return _locations;
}

const std::set<std::string> &ServerConfig::getAllowedMethods() const
{
	return _allowed_methods;
}

const std::string	&ServerConfig::getIndex() const
{
	return _index;
}

const bool &ServerConfig::getAutoindex() const
{
	return _autoindex;
}

// Setters

void ServerConfig::setServerName(std::string server_name)
{
	_server_name = server_name;
}

void ServerConfig::setPorts(std::vector<int> ports)
{
	_ports = ports;
}

void ServerConfig::setHostnames(std::vector<std::string> hostnames)
{
	_hostnames = hostnames;
}

void ServerConfig::setRoot(std::string root)
{
	_root = root;
}

void ServerConfig::setCgiParam(std::map<std::string, std::string> cgi_param)
{
	_cgi_param = cgi_param;
}

void ServerConfig::setCgiPass(std::string cgi_pass)
{
	_cgi_pass = cgi_pass;
}

void ServerConfig::setAutoindex(bool autoindex)
{
	_autoindex = autoindex;
}
