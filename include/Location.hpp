/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ycardona <ycardona@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 15:06:23 by elias             #+#    #+#             */
/*   Updated: 2023/12/05 16:17:34 by ycardona         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <vector>
#include <iostream>
#include <set>
#include <map>

# include <iostream>
# include <iomanip>
# include <sstream>
# include <fstream>
# include <string>
# include <limits>
# include <cstdio>

# include "LogMsg.hpp"

class Location
{
	private:
	//Location variables
		std::string							_path;
		std::set<std::string>				_allowed_methods;
		std::string							_alias;
		bool								_aliasSet;
		std::string							_root;
		std::map<std::string, std::string>	_cgi_param;
		std::string							_cgi_pass;
		bool								_autoindex;
		std::string							_index;
	public:
		Location();
		Location(const Location &src);
		Location &operator=(const Location &rhs);
		~Location();

		static Location	createLocation(std::string location_str);

	//Getters
		const std::string			&getLocationPath() const;
		const std::set<std::string>	&getLocationAllowedMethods() const;
		const std::string			&getLocationAlias() const;
		const bool					&getLocationAliasSet() const;
		const std::string			&getLocationRoot() const;
		const std::map<std::string, std::string> &getLocationCgiParam() const;
		const std::string			&getLocationCgiPass() const;
		const bool 					&getLocationAutoindex() const;
		const std::string			&getLocationIndex() const;
	//Setters
		void	addAllowedMethod(std::string const &method);
		void	setAutoIndex(bool autoindex);
		void	setPath(std::string path);
		void	setAllowedMethods(std::set<std::string> allowed_methods);
		void	setAlias(std::string alias);
		void	setAliasSet(bool aliasSet);
		void	setRoot(std::string root);
		void	setCgiParam(std::map<std::string, std::string> cgi_param);
		void	addCgiParam(std::string key, std::string value);
		void	setCgiPass(std::string cgi_pass);
};

#endif