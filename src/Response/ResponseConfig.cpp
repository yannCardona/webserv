#include "Response.hpp"

void	Response::setConfigs()
{
	_root = _virtual_server.getRoot();
	_error_page = _virtual_server.getErrorPage();
	_allowed_methods = _virtual_server.getAllowedMethods();
	_client_body_buffer_size = _virtual_server.getClientBodyBufferSize();
	_index = _virtual_server.getIndex();
	_autoindex = _virtual_server.getAutoindex();
	_cgi_param = _virtual_server.getCgiParam();
	_cgi_pass = _virtual_server.getCgiPass();
}

void	Response::setConfigsLocation()
{
	if (!_location.getLocationAllowedMethods().empty())
		_allowed_methods = _location.getLocationAllowedMethods();
	if (_location.getLocationAliasSet()) {
		size_t pos = _request_target.find(_location.getLocationPath());
   		if (pos != std::string::npos) {
        	_request_target.replace(pos, _location.getLocationPath().length(), _location.getLocationAlias());
		}
	}
	if (!_location.getLocationRoot().empty())
		_root = _location.getLocationRoot();
	if (!_location.getLocationCgiParam().empty())
		_cgi_param = _location.getLocationCgiParam();
	if (!_location.getLocationCgiPass().empty())
		_cgi_pass = _location.getLocationCgiPass();
	_autoindex = _location.getLocationAutoindex();
	if (!_location.getLocationIndex().empty())
		_index = _location.getLocationIndex();
}

void	Response::selectLocation()
{
	std::vector<std::string>::iterator it;
	std::string	location_match;

	for (size_t i = 0; i < _virtual_server.getLocationPath().size(); i++)
	{
		std::string	current_location_path = _virtual_server.getLocationPath()[i];
		
		//check exact matching witch request target "="
		if (2 < current_location_path.length() && current_location_path[0] == '='
			&& current_location_path.substr(2) == _request_target)
		{
			location_match = current_location_path;
			break ;
		}
		//check for matching file extension
		if (current_location_path.compare(0, 4, "~ \\.") == 0 && 4 < current_location_path.length())
		{
			if (current_location_path[current_location_path.length() - 1] != '$') {
				LOG_MSG(LOG_ERROR, BG_RED, "Error: missing '$' at the end of location");
			}
			else {
				int i = 4;
				bool	found = false;
				if (current_location_path[4] == '(')
					i++;
				while (1) {
					std::string	extension = ".";
					while (current_location_path[i] != '$' && current_location_path[i] != '|' && current_location_path[i] != ')') {
						extension += current_location_path[i];
						i++;
					}
					if (_request_target.find(extension) != std::string::npos) {
						location_match = current_location_path;
						found = true;
						break ;
					}
					if (current_location_path[i] == '$' || current_location_path[i] == ')')
						break ;
					else 
						i++;			
				}
				if (found)
					break ;
			}
		}
		//check for matching substring
		if (_request_target.find(current_location_path) != std::string::npos
			&& location_match.size() < current_location_path.size())
			location_match = current_location_path;
	}
	if (!location_match.empty()) {
		_location = _virtual_server.getLocations()[location_match];
		setConfigsLocation();
	}
}