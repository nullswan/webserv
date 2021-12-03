#include "conf/parser.hpp"
#include "server/poll.hpp"

int	main(int ac, char **av) {
	Webserv::Conf::Parser	parser;
	if (!parser.run(ac, av)) {
		return 1;
	}

	Webserv::Server::Poll poll;
	try {
		poll.init(parser.get_servers());
		parser.clear();
		return poll.run();
	} catch (std::exception &e) {
		parser.clear();

		std::cerr << "fatal: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
