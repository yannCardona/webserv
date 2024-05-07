/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ycardona <ycardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 15:16:09 by elias             #+#    #+#             */
/*   Updated: 2023/12/04 17:53:11 by ycardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() : _aliasSet(false), _autoindex(false) {}

Location::~Location() {}

Location::Location(const Location &other)
{
	*this = other;
}

Location &Location::operator=(const Location &other)
{
	if (this != &other)
	{
		_path = other._path;
		_allowed_methods = other._allowed_methods;
		_alias = other._alias;
		_aliasSet = other._aliasSet;
		_root = other._root;
		_cgi_pass = other._cgi_pass;
		_cgi_param = other._cgi_param;
		_autoindex = other._autoindex;
		_index = other._index;
	}
	return *this;
}

const std::string &Location::getLocationPath() const
{
	return _path;
}

const std::set<std::string> &Location::getLocationAllowedMethods() const
{
	return _allowed_methods;
}

const std::string &Location::getLocationAlias() const
{
	return _alias;
}

const bool &Location::getLocationAliasSet() const
{
	return _aliasSet;
}

const std::string &Location::getLocationRoot() const
{
	return _root;
}

const std::map<std::string, std::string> &Location::getLocationCgiParam() const
{
	return _cgi_param;
}

const std::string &Location::getLocationCgiPass() const
{
	return _cgi_pass;
}

const bool &Location::getLocationAutoindex() const
{
	return _autoindex;
}

const std::string	&Location::getLocationIndex() const {
	return _index;
}

void Location::setPath(std::string path)
{
	_path = path;
}

void Location::setAllowedMethods(std::set<std::string> allowed_methods)
{
	_allowed_methods = allowed_methods;
}

void Location::setAlias(std::string alias)
{
	_alias = alias;
}

void Location::setAliasSet(bool aliasSet)
{
	_aliasSet = aliasSet;
}

void Location::setRoot(std::string root)
{
	_root = root;
}

void Location::setAutoIndex(bool autoindex)
{
	_autoindex = autoindex;
}

void Location::setCgiParam(std::map<std::string, std::string> cgi_param)
{
	_cgi_param = cgi_param;
}

void Location::addCgiParam(std::string key, std::string value)
{
	_cgi_param.insert(std::make_pair(key, value));
}

void	Location::setCgiPass(std::string cgi_pass)
{
	_cgi_pass = cgi_pass;
}

void	Location::addAllowedMethod(std::string const &method)
{
	_allowed_methods.insert(method);
}
