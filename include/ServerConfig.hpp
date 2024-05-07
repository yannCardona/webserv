/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbaumann <lbaumann@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 08:07:38 by elias             #+#    #+#             */
/*   Updated: 2024/01/17 11:15:39 by lbaumann         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "Location.hpp"

# include <set>
# include <vector>
# include <iostream>

# include <iostream>
# include <iomanip>
# include <sstream>
# include <fstream>
# include <string>
# include <limits>
# include <cstdio>
# include <stdexcept>
# include <map>


class ServerConfig
{
	private:
		bool								_autoindex;
		unsigned long						_client_body_buffer_size;
		std::string							_server_name;
		std::string							_root;
		std::string							_cgi_pass;
		std::string							_index;
		std::vector<int> 					_ports;
		std::vector<std::string>			_hostnames;
		std::vector<std::string>			_location_path;
		std::set<std::string>				_allowed_methods;
		std::map<std::string, std::string>	_error_page;
		std::map<std::string, std::string>	_cgi_param;
		std::map<std::string, Location>		_locations;

	public:
		ServerConfig();
		ServerConfig(const ServerConfig &src);
		ServerConfig &operator=(const ServerConfig &rhs);
		~ServerConfig();
		
		//Config parsing
		static std::vector<ServerConfig>	parseConfigFile(const std::string& filename) throw (std::runtime_error);
		static std::vector<ServerConfig>	completeConfig(const std::string& filename, std::vector<ServerConfig> serverConfigs);
		static std::vector<ServerConfig>	splitConfig(std::vector<ServerConfig> serverConfigs);
		static int							checkDouble(std::vector<ServerConfig> serverConfigs);
		static void							printConfig(std::vector<ServerConfig> serverConfigs);

		//Getters
		const bool									&getAutoindex() const;
		const int									&getPort() const;
		const unsigned long							&getClientBodyBufferSize() const;
		const std::string							&getServerName() const;
		const std::string							&getHostname() const;
		const std::string							&getRoot() const;
		const std::string							&getIndex() const;
		const std::string							&getCgiPass() const;
		const std::vector<int>						&getPorts() const;
		const std::vector<std::string>				&getHostnames() const;
		const std::vector<std::string>				&getLocationPath() const;
		const std::set<std::string>					&getAllowedMethods() const;
		const std::map<std::string, std::string>	&getErrorPage() const;
		const std::map<std::string, std::string>	&getCgiParam() const;
		std::map<std::string, Location>				&getLocations();
		
		//Setters
		void	setServerName(std::string server_name);
		void	setHostnames(std::vector<std::string> hostnames);
		void	setPorts(std::vector<int> ports);
		void	setRoot(std::string root);
		void	setCgiParam(std::map<std::string, std::string> cgi_param);
		void	setCgiPass(std::string cgi_pass);
		void	setAutoindex(bool autoindex);
};

#endif