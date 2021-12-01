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
	typedef std::vector<IServer *> IServerList;

 private:
	std::vector<IServer *>	_servers;
	std::string _conf_file_path;
	std::string	_conf_file;

 public:
	Parser() : _conf_file_path("") {
	}

	~Parser() { clear(); }

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

	IServerList	&getServers() {
		return _servers;
	}
	bool	_check_file_flags() {
		struct stat buffer;
		if (stat(_conf_file_path.c_str(), &buffer) != 0)
			return file_not_found_error(_conf_file_path);
		else if (buffer.st_mode & S_IFDIR)
			return file_is_directory_error(_conf_file_path);

		if (access(_conf_file_path.c_str(), F_OK) == -1)
			return insufficient_permission_error(_conf_file_path);

		return true;
	}
	inline void	_skip_whitespaces(std::string *s) {
		while ((*s)[0] == '\t' || (*s)[0] == ' ')
			s->erase(0, 1);
	}
};
}  // namespace Conf
}  // namespace Webserv

#endif  // CONF_PARSER_HPP_
