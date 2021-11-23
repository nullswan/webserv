/*
	Read configuration file provided in argument.
		-> Otherwise, raise an error.
	
	Parse the configuration file and fill the _instances member.
		-> In case of multiple host/port pair, an error will be raised.

	Instance object is copied into Poll object, so the allocation remain.
*/

#ifndef CONF_PARSER_HPP_
#define CONF_PARSER_HPP_

#include <string>
#include <vector>

#include "../models/IServer.hpp"

namespace Webserv {
namespace Conf {
class Parser {
 public:
	typedef Webserv::Models::IServer IServer;

 private:
	std::vector<IServer *>	_servers;

 public:
	Parser(int ac, char **av) {
		(void)ac;
		(void)av;

		/*
			Inject a default server
		*/
		_servers.push_back(new IServer("local-net 1", "0.0.0.0", 8080));
		_servers.push_back(new IServer("local-net 2", "0.0.0.0", 8081));
		_servers.push_back(new IServer("local-net 3", "0.0.0.0", 8082));
	}
	~Parser() {
		clear();
	}

	/*
		Return false on duplicate or parsing error.
	*/
	bool	run() { return true; }

	void clear() {
		if (_servers.size() == 0)
			return;

		std::vector<IServer *>::iterator it = _servers.begin();
		for (; it != _servers.end(); it++) {
			delete *it;
		}
		_servers.clear();
	}

	std::vector<Webserv::Models::IServer *>	&getServers() {
		return _servers;
	}
};
}  // namespace Conf
}  // namespace Webserv

#endif  // CONF_PARSER_HPP_
