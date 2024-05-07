#include "Server.hpp"

volatile bool g_runServer = true;;

/**
 * g_runServer ignored at the moment, because select throws an exception
 * once the signal function is called when a SIGINT is received
*/
void catcher(int sig)
{
	(void)sig;
	std::cout << "SIGINT received" << std::endl;
	g_runServer = false;
}

int	main(int argc, char* argv[]) {
	if (argc > 2) {
		LOG_MSG(LOG_ERROR, BG_RED, "Too many arguments. Usage: "
		<< "./webserv [<config_file>]");
		return (EXIT_SUCCESS);
	}
	Server server;
	struct sigaction sact;


	sigemptyset(&sact.sa_mask);
	sact.sa_flags = 0;
	sact.sa_handler = catcher;
	sigaction(SIGINT, &sact, NULL);
	try {
		if (argc == 2)
			server.init(argv[1]);
		else
			server.init(DEFAULT_CONFIG);
	} catch (std::exception& e) {
		LOG_MSG(LOG_DEBUG, BG_RED, "Error in server init: " << e.what());
		return (EXIT_FAILURE);
	}
	
	try {
		server.run();
	} catch (std::exception& e) {
		LOG_MSG(LOG_DEBUG, BG_RED, "Error in server run: " << e.what());
		server.exitServer();
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
