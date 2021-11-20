#include "conf/parser.hpp"
#include "server/poll.hpp"

int	main(int ac, char **av) {
	Webserv::Conf::Parser	parser(ac, av);
	if (!parser.run()) {
		return (1);
	}

	try {
		Webserv::Server::Poll poll(parser.getServers());
		parser.clear();
		return poll.run();
	} catch (std::exception &e) {
		parser.clear();

		std::cerr << "fatal:" << e.what() << std::endl;
		return (1);
	}
}
