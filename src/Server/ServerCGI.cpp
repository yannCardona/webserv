#include "Server.hpp"

void	Server::_receiveFromCGI(Client& client){
	char			buffer[RECV_BUFFER];
	ssize_t			bytesRead;
	int				pipeToChild = client.getReadPipeFromChild();
	std::string		readFromChild;
	int status;
	pid_t result;

	std::memset(buffer, 0, sizeof(buffer));
	bytesRead = read(pipeToChild, buffer, sizeof(buffer));
	if (bytesRead == -1) {
		LOG_MSG(LOG_ERROR, BG_RED, "read from child error");
		_closeConnection(pipeToChild);
		return;
	}

	result = waitpid(client.getChildPID(), &status, WNOHANG);
	if (result == 0) {
		LOG_MSG(LOG_DEBUG, BG_WHITE, "Child:" << client.getChildPID() << " still running"
			<< ", continue reading");
	} else if (result == -1) {
		LOG_MSG(LOG_ERROR, BG_RED, "waitpid error");
		_closeConnection(pipeToChild);
		return;
	} else { //we first go in here to check if bytesRead == 0 when we are sure that the child finished executing
		if (WEXITSTATUS(status)) { //we go here when a child returns smth other than 0 (indicating an error)
			LOG_MSG(LOG_DEBUG, BG_WHITE, "Child: " << client.getChildPID() << " exited abnormally with " << WEXITSTATUS(status));
			LOG_MSG(LOG_DEBUG, BG_WHITE, "setting custom error page");
			_rmFromSet(pipeToChild, &_RmasterSet);
			close(pipeToChild);
			_addToSet(client.getSocket(), &_WmasterSet);
			client.setErrorPage("500", "Internal Server Error");
			return;
		} else {
			LOG_MSG(LOG_DEBUG, BG_WHITE, "Child: " << client.getChildPID() << " exited normally with " << WEXITSTATUS(status));
		} 	
		if (bytesRead == 0) {
			LOG_MSG(LOG_DEBUG, BG_WHITE, "Reading from child finished");
			_rmFromSet(pipeToChild, &_RmasterSet);
			close(pipeToChild);
			_addToSet(client.getSocket(), &_WmasterSet);
			return;
		}
	}
	
	

	readFromChild.append(buffer, bytesRead);
	client.feedToResponseStr(readFromChild);
}

void	Server::_sendToCGI(Client& client){
	// char			buffer[10];
	ssize_t			bytesSent;
	int				pipeToChild = client.getWritePipeToChild();
	std::string		httpRequestBody = client.getRequest().getRequestBody();

	// std::memset(buffer, 0, sizeof(buffer));
	// if (httpRequestBody.length() > sizeof(buffer)) {
	// 	std::memcpy(buffer, httpRequestBody.c_str(), sizeof(buffer));
	// 	bytesSent = write(pipeToChild, buffer, sizeof(buffer));
	// } else {
	// 	std::memcpy(buffer, httpRequestBody.c_str(), httpRequestBody.length());
	// 	bytesSent = write(pipeToChild, buffer, httpRequestBody.length());
	// }
	// std::memcpy(buffer, httpRequestBody.c_str(), httpRequestBody.length());
	LOG_MSG(LOG_DEBUG, BG_WHITE, "SENDING TO CGI");

	bytesSent = write(pipeToChild, httpRequestBody.c_str(), httpRequestBody.length());
	if (bytesSent == -1) {
		LOG_MSG(LOG_ERROR, BG_RED, "write to child error");
		_closeConnection(pipeToChild);
		return;
	}

	if (static_cast<unsigned long>(bytesSent) == httpRequestBody.length()) {
		_rmFromSet(pipeToChild, &_WmasterSet);
		close(pipeToChild);
		_addToSet(client.getReadPipeFromChild(), &_RmasterSet);
	} else {
		client.getRequest().updateRequestBody(httpRequestBody.substr(bytesSent));
	}
}
