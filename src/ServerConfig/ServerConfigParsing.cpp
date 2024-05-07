#include "ServerConfig.hpp"

int stringToInteger(const std::string& str)
{
	std::istringstream iss(str);
	int result;
	iss >> result;
	return result;
}

void ServerConfig::printConfig(std::vector<ServerConfig> serverConfigs)
{
	for (size_t j = 0; j < serverConfigs.size(); j++)
	{
		std::cout << "Server Name: " << serverConfigs[j]._server_name << std::endl;
		std::cout << "Number of servers: " << serverConfigs.size() << std::endl;

		std::cout << "Hostnames: ";
		for (size_t i = 0; i < serverConfigs[j]._hostnames.size(); i++)
		{
				std::cout << serverConfigs[j]._hostnames[i] << std::endl;
		}

		std::cout << "Ports: ";
		for (size_t i = 0; i < serverConfigs[j]._ports.size(); i++)
		{
				std::cout << serverConfigs[j]._ports[i] << std::endl;
		}
		std::cout << "##########################LOCATIONS##########################" << std::endl;
		std::cout << "Location: ";
		std::cout << "Number of locations: " << serverConfigs[j].getLocations().size() << std::endl;	
		// for (size_t i = 0; i < serverConfigs[j]._locations.size(); i++)
		// {
		// 	std::cout << "LOCATION: " << i << "####################################"  << std::endl;
		// 	std::cout << "Path: " << serverConfigs[j]._location_path[i] << std::endl;
		// 	std::cout << "Root: " << serverConfigs[j]._locations[i]._root << std::endl;
		// 	std::cout << "Autoindex: " << serverConfigs[j]._locations[i]._autoindex << std::endl;
		// 	std::cout << "Alias: " << serverConfigs[j]._locations[i]._alias << std::endl;
		// 	std::cout << "AliasSet: " << serverConfigs[j]._locations[i]._aliasSet << std::endl;
		// 	std::cout << "CgiPass: " << serverConfigs[j]._locations[i]._cgi_pass << std::endl;
		// 	std::cout << "CgiParam: " << std::endl;
		// 	for (std::map<std::string, std::string>::const_iterator it = serverConfigs[j]._locations[i]._cgi_param.begin(); it != serverConfigs[j]._locations[i]._cgi_param.end(); ++it)
		// 	{
		// 		std::cout << "Key: " << it->first << " Value: " << it->second << std::endl;
		// 	}
		// 	std::cout << "Allowed Methods:" << std::endl;
		// 	for (std::set<std::string>::const_iterator it = serverConfigs[j]._locations[i]._allowed_methods.begin(); it != serverConfigs[j]._locations[i]._allowed_methods.end(); ++it)
		// 	{
		// 		std::cout << *it << " ";
		// 	}
		// 	std::cout << std::endl;
		// 	std::cout << "Client_Body_Size: " << serverConfigs[j]._locations[i]._client_body_buffer_size << std::endl;
		// }
		std::cout << "#############################################################" << std::endl;
		std::cout << "Allowed Methods:" << std::endl;
		for (std::set<std::string>::const_iterator it = serverConfigs[j]._allowed_methods.begin(); it != serverConfigs[j]._allowed_methods.end(); ++it)
		{
			std::cout << *it << " ";
		}
		std::cout << std::endl;
		std::cout << "Error Pages:" << std::endl;

		for (std::map<std::string, std::string>::const_iterator it = serverConfigs[j]._error_page.begin(); it != serverConfigs[j]._error_page.end(); ++it)
		{
			std::cout << "Error Code: " << it->first << " Page: " << it->second << std::endl;
		}
		std::cout << std::endl;
		std::cout << "Client_Body_Size: " << serverConfigs[j]._client_body_buffer_size << std::endl;
		std::cout << "root: " << serverConfigs[j]._root << std::endl;
			std::cout << "index: " << serverConfigs[j]._index << std::endl;
		std::cout << "autoindex: " << serverConfigs[j]._autoindex << std::endl;

		std::cout << std::endl;
	}
}

std::vector<ServerConfig> ServerConfig::parseConfigFile(const std::string& filename) throw (std::runtime_error)
{
	std::string dir;
	std::vector<ServerConfig> serverConfigs;
	std::ifstream configFile(filename.c_str());

	if (!configFile.is_open())
	{
		std::cerr << "Error: Unable to open the configuration file." << std::endl;
		return serverConfigs;
	}
	std::string line;
	ServerConfig currentServerConfig;

	while (std::getline(configFile, line))
	{
		std::istringstream iss(line);
		std::string directive;
		iss >> directive;
		if (line.empty() || line[0] == '#')
		{
			continue;
		}
		if (directive == "server")
		{
			if (dir == "}")
			{
				serverConfigs.push_back(currentServerConfig);
			}
			currentServerConfig = ServerConfig();
		}
		else if (directive == "server_name")
		{
				iss >> currentServerConfig._server_name;
		}
		else if (directive == "listen")
		{
			std::string port;
			while (iss >> port)
			{
				int portValue = stringToInteger(port);
				currentServerConfig._ports.push_back(portValue);
			}
		}
		else if (directive == "host_name")
		{
			std::string hostname;
			while (iss >> hostname)
			{
				currentServerConfig._hostnames.push_back(hostname);
			}
		}
		else if (directive == "root")
		{
			iss >> currentServerConfig._root;
		}
		else if (directive == "error_page")
		{
			std::string errorCode;
			std::string errorPage;
			iss >> errorCode >> errorPage;
			currentServerConfig._error_page[errorCode] = errorPage;
		}
		else if (directive == "client_body_buffer_size")
		{
			iss >> currentServerConfig._client_body_buffer_size;
		}
		else if (directive == "cgi_param")
		{
			std::string key, value;
			iss >> key >> value;
			currentServerConfig._cgi_param[key] = value;
		}
		else if (directive == "cgi_pass")
		{
			iss >> currentServerConfig._cgi_pass;
		}
		else if (directive == "location")
		{
			size_t locationPos = line.find("location");
			size_t openBracePos = line.find("{");
			std::string	path;
			if (locationPos != std::string::npos && openBracePos != std::string::npos)
			{
				path = line.substr(locationPos + 8, openBracePos - locationPos - 8);

				size_t firstNotSpace = path.find_first_not_of(" \t\n\v\f\r");
				size_t lastNotSpace = path.find_last_not_of(" \t\n\v\f\r");
				if (firstNotSpace != std::string::npos && lastNotSpace != std::string::npos)
				{
					path = path.substr(firstNotSpace, lastNotSpace - firstNotSpace + 1);
				}
				currentServerConfig._location_path.push_back(path);
			}
			std::string locationData;
			while (std::getline(configFile, line) && line.find('}') == std::string::npos)
			{
				locationData += line + "\n";
			}
			Location	new_location = Location::createLocation(locationData);
			new_location.setPath(path);
			currentServerConfig._locations.insert(std::make_pair(path, new_location));
		}
		else if (directive == "allow_methods")
		{
			std::string method;
			while (iss >> method)
			{
				currentServerConfig._allowed_methods.insert(method);
			}
		}
		else if (directive == "index")
		{
			iss >> currentServerConfig._index;
		}
		else if (directive == "autoindex")
		{
			std::string autoindex;
			iss >> autoindex;
			if (autoindex == "on")
			{
				currentServerConfig._autoindex = true;
			}
			else if (autoindex == "off")
			{
				currentServerConfig._autoindex = false;
			}
		}
		dir = directive;
	}
	if (dir == "}") 
	{
	serverConfigs.push_back(currentServerConfig);
	}
	else
		throw (std::runtime_error("Error in parseConfigFile"));
	configFile.close();
	return serverConfigs;
}

std::vector<ServerConfig> ServerConfig::completeConfig(const std::string& filename, std::vector<ServerConfig> serverConfigs)
{
	std::vector<ServerConfig> defaultConfigs;
	defaultConfigs = parseConfigFile(filename);
	// printConfig(defaultConfigs);
	for (size_t i = 0; i < serverConfigs.size(); i++)
	{
		if (serverConfigs[i]._ports.empty())
		{
			serverConfigs[i]._ports = defaultConfigs[0]._ports;
		}
		if (serverConfigs[i]._hostnames.empty())
		{
			serverConfigs[i]._hostnames = defaultConfigs[0]._hostnames;
		}
		if (serverConfigs[i]._allowed_methods.empty())
		{
			serverConfigs[i]._allowed_methods = defaultConfigs[0]._allowed_methods;
		}
		if (serverConfigs[i]._client_body_buffer_size == 0)
		{
			serverConfigs[i]._client_body_buffer_size = defaultConfigs[0]._client_body_buffer_size;
		}
		if (serverConfigs[i]._index.empty())
		{
			serverConfigs[i]._index = defaultConfigs[0]._index;
		}
		if (serverConfigs[i]._server_name.empty())
		{
			serverConfigs[i]._server_name = defaultConfigs[0]._server_name;
		}
		for (std::map<std::string, std::string>::const_iterator it = defaultConfigs[0]._error_page.begin(); it != defaultConfigs[0]._error_page.end(); ++it)
		{
			if (serverConfigs[i]._error_page.find(it->first) == serverConfigs[i]._error_page.end())
			{
				serverConfigs[i]._error_page[it->first] = it->second;
			}
		}
	}
	return serverConfigs;
}

std::vector<ServerConfig> ServerConfig::splitConfig(std::vector<ServerConfig> serverConfigs)
{
	std::vector<ServerConfig> splitConfigs;
	for (size_t i = 0; i < serverConfigs.size(); i++)
	{
		if (serverConfigs[i]._ports.size() > 1 || serverConfigs[i]._hostnames.size() > 1)
		{
			for (size_t j = 0; j < serverConfigs[i]._ports.size(); j++)
			{
				for (size_t k = 0; k < serverConfigs[i]._hostnames.size(); k++)
				{
					ServerConfig newConfig = serverConfigs[i];
					newConfig._ports.clear();

					newConfig._ports.push_back(serverConfigs[i]._ports[j]);

					newConfig._hostnames.clear();
					newConfig._hostnames.push_back(serverConfigs[i]._hostnames[k]);

					splitConfigs.push_back(newConfig);
				}
			}
		}
		else
		{
			splitConfigs.push_back(serverConfigs[i]);
		}
	}
	return splitConfigs;
}

int ServerConfig::checkDouble(std::vector<ServerConfig> serverConfigs)
{
   std::set<std::pair<std::string, int> > serverSet;  // Pair of hostname and port for uniqueness check
    for (std::vector<ServerConfig>::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
    {
        for (std::vector<int>::const_iterator portIt = it->_ports.begin(); portIt != it->_ports.end(); ++portIt)
        {
            for (std::vector<std::string>::const_iterator hostIt = it->_hostnames.begin(); hostIt != it->_hostnames.end(); ++hostIt)
            {
                std::pair<std::string, int> serverKey = std::make_pair(*hostIt, *portIt);
                if (serverSet.find(serverKey) != serverSet.end())
                {
                    // Duplicate hostname and port found
					LOG_MSG(LOG_ERROR, BG_RED, "Error: Duplicate hostname and port found in server configurations.");
					return 1;
                }
                serverSet.insert(serverKey);
            }
        }
    }

    // If no duplicates, return the original server configurations
    return 0;
}