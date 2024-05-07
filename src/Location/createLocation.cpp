#include "Location.hpp"

Location	Location::createLocation(std::string location_str)
{
	Location			new_location;
	std::istringstream	iss(location_str);
	std::string			line;

	while (std::getline(iss, line))
	{
		if (line.empty() || line[0] == '#')
			continue;

		std::istringstream lineIss(line);
		std::string directive;
		lineIss >> directive;

		if (directive == "alias")
		{
			new_location._aliasSet = true;
			lineIss >> new_location._alias;
		}
		else if (directive == "allow_methods")
		{
			std::string method;
			while (lineIss >> method)
				new_location.addAllowedMethod(method);
		}
		else if (directive == "root")
		{
			lineIss >> new_location._root; 
		}
		else if (directive == "cgi_param")
		{
			std::string key, value;
			lineIss >> key >> value;
			new_location.addCgiParam(key, value);
		}
		else if (directive == "cgi_pass")
		{
			lineIss >> new_location._cgi_pass;
		}
		else if (directive == "index")
		{
			lineIss >> new_location._index;
		}
		else if (directive == "autoindex")
		{
			std::string autoindex;
			lineIss >> autoindex;
			if (autoindex == "on")
				new_location.setAutoIndex(true);
			else if (autoindex == "off")
				new_location.setAutoIndex(false);
		}
	}
	return new_location;
}
