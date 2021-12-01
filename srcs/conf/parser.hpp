/*
	Read configuration file provided in argument.
		-> Otherwise, raise an error.
	
	Parse the configuration file and fill the _instances member.
		-> In case of multiple host/port pair, an error will be raised.

	Instance object is copied into Poll object, so the allocation remain.
*/

#ifndef CONF_PARSER_HPP_
#define CONF_PARSER_HPP_

#include <unistd.h>
#include <sys/stat.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "enums.hpp"
#include "errors.hpp"
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
		// (void)ac;
		// (void)av;
		// _servers.push_back(new IServer("local-net 1", "0.0.0.0", 8080));
		// _servers.push_back(new IServer("local-net 2", "0.0.0.0", 8081));
		// _servers.push_back(new IServer("local-net 3", "0.0.0.0", 8082));
	}

	~Parser() { clear(); }

	// ToDo: Must handle duplicate
	bool	run(int ac, char **av) {
		if (!_verify_arguments(ac, av))
			return false;
		if (!_dump_configuration())
			return false;

		try {
			if (!_parse_configuration())
				return false;
		} catch (std::exception &e) {
			return false;
		}
		return true;
	}

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

 private:
	bool	_verify_arguments(int ac, char **av) {
		if (ac >= 3)
			return usage_error(av[0]);
		if (ac == 1) {
			no_config_file_error();
			return use_default_configuration();
		}

		_conf_file_path = av[1];
		if (_conf_file_path.substr(
			_conf_file_path.size() - 5, _conf_file_path.size()) != ".conf")
			return config_file_extension_error(av[1]);

		return _check_file_flags();
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

	bool	_dump_configuration() {
		if (_conf_file_path == "") {
			_conf_file = DEFAULT_CONF_FILE;
			std::cout << _conf_file << std::endl;
			return true;
		} else {
			std::ifstream	fs;

			fs.open(_conf_file_path.c_str());
			if (!fs || !fs.is_open())
				return cannot_open_error(_conf_file_path);

			fs.seekg(0, fs.end);
			int length = fs.tellg();
			fs.seekg(0, fs.beg);

			char *buffer = new char[length];
			fs.read(buffer, length);
			_conf_file = buffer;

			fs.close();
			delete []buffer;
			return true;
		}
		return false;
	}

	int	_resolve_line(std::string *line, const int &scope) {
		if (line->substr(0, 8) == "server {")
			return CONF_SERVER_OPENING;
		_skip_whitespaces(line);
		if (line->size() == 0)
			return CONF_EMPTY_TOKEN;
		if (scope <= 0)
			return CONF_ERRORENOUS_TOKEN;

		if (line->size() == 1 && (*line)[0] == '}')
			return CONF_BLOCK_CLOSING;

		std::string tabs_key = line->substr(0, line->find("\t"));
		std::string space_key = line->substr(0, line->find(" "));

		return _resolve_keys(tabs_key, space_key);
	}

	int
	_resolve_keys(const std::string &first_key, const std::string &second_key) {
		int state = _resolve_key(first_key);
		if (state != CONF_NOT_FOUND_TOKEN)
			return state;
		return _resolve_key(second_key);
	}

	int _resolve_key(const std::string &key) {
		if (key == "server_name")
			return CONF_SERVER_NAME;
		if (key == "index")
			return CONF_SERVER_INDEX;
		if (key == "allowed_methods")
			return CONF_BLOCK_ALLOWED_METHODS;
		if (key == "redirect")
			return CONF_BLOCK_REDIRECT;
		if (key == "location")
			return CONF_SERVER_LOCATION;
		return CONF_NOT_FOUND_TOKEN;
	}

	inline void	_skip_whitespaces(std::string *s) {
		while ((*s)[0] == '\t' || (*s)[0] == ' ')
			s->erase(0, 1);
	}

	void _extract_value(const std::string &key, std::string *bucket) {
		bucket->erase(0, key.size());
		_skip_whitespaces(bucket);
		if ((*bucket)[bucket->size() - 1] != ';') {
			invalid_delimiter_error(*bucket);
			throw std::runtime_error("invalid delimiter");
		}
		bucket->erase(bucket->size() - 1, 1);
	}

	bool	_parse_configuration() {
		std::string line;
		std::stringstream	ss(_conf_file);

		int token = 0, scope = 0, line_count = 0;
		while (std::getline(ss, line) && ++line_count) {
			token = _resolve_line(&line, scope);
			switch (token) {
				case CONF_EMPTY_TOKEN:
					continue;
				case CONF_SERVER_OPENING: {
					_servers.push_back(new IServer());
					++scope;
					continue;
				}
				case CONF_SERVER_NAME: {
					_extract_value("server_name", &line);
					_servers.back()->set_name(line);
					continue;
				}
				case CONF_SERVER_INDEX: {
					_extract_value("index", &line);
					while (line.find(" ") != std::string::npos) {
						_servers.back()->add_index(line.substr(0, line.find(" ")));
						line.erase(0, line.find(" ") + 1);
						_skip_whitespaces(&line);
					}
					if (line.size() > 1)
						_servers.back()->add_index(line);
					continue;
				}
				case CONF_BLOCK_ALLOWED_METHODS: {
					_extract_value("allowed_methods", &line);
					std::string method;
					while (line.find(" ") != std::string::npos) {
						method = line.substr(0, line.find(" "));
						if (Models::get_method(method) == Models::METHOD_UNKNOWN)
							return unknown_method_error(method);
						_servers.back()->set_method(Models::get_method(method), true);
						line.erase(0, line.find(" ") + 1);
					}
					if (Models::get_method(line) == Models::METHOD_UNKNOWN)
							return unknown_method_error(line);
					_servers.back()->set_method(Models::get_method(method), true);
				}
				default:
					std::cout << "tk: " << token << " | " << line << std::endl;
			}
		}
		return true;
	}
};
}  // namespace Conf
}  // namespace Webserv

#endif  // CONF_PARSER_HPP_
