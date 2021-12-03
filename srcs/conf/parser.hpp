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
#include <stdlib.h>
#include <sys/stat.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "enums.hpp"
#include "errors.hpp"
#include "../models/IServer.hpp"
#include "../models/ILocation.hpp"

namespace Webserv {
namespace Conf {
class Parser {
 public:
	typedef Webserv::Models::IServer	IServer;
	typedef Webserv::Models::ILocation	ILocation;

	typedef std::vector<IServer *> IServerList;

 private:
	std::vector<IServer *>	_servers;
	std::string _conf_file_path;
	std::string	_conf_file;

 public:
	Parser() : _conf_file_path("") {}
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

	IServerList	&get_servers() {
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
			return true;
		} else {
			std::ifstream	fs;

			fs.open(_conf_file_path.c_str());
			if (!fs || !fs.is_open())
				return cannot_open_error(_conf_file_path);

			fs.seekg(0, fs.end);
			int length = fs.tellg();
			fs.seekg(0, fs.beg);

			char *buffer = new char[length + 1];
			for (int i = 0; i < length + 1; i++)
				buffer[i] = 0;
			fs.read(buffer, length);
			_conf_file = buffer;

			fs.close();
			delete []buffer;
			return true;
		}
		return false;
	}

	int	_resolve_line(std::string *line) {
		if (line->substr(0, 8) == "server {")
			return CONF_SERVER_OPENING;
		_skip_whitespaces(line);
		if (line->size() == 0)
			return CONF_EMPTY_TOKEN;
		if ((*line)[0] == '}' && line->size() == 1)
			return CONF_BLOCK_CLOSING;
		return _resolve_keys(line->substr(0, line->find("\t")),
			line->substr(0, line->find(" ")));
	}

	int
	_resolve_keys(const std::string &first_key, const std::string &second_key) {
		int state = _resolve_key(first_key);
		if (state != CONF_NOT_FOUND_TOKEN)
			return state;
		if ((state = _resolve_key(second_key)) != CONF_NOT_FOUND_TOKEN)
			return state;
		return CONF_ERRORENOUS_TOKEN;
	}

	int _resolve_key(const std::string &key) {
		if (key == "allowed_methods")
			return CONF_BLOCK_ALLOWED_METHODS;
		if (key == "autoindex")
			return CONF_BLOCK_AUTOINDEX;
		if (key == "body_limit")
			return CONF_BLOCK_BODY_LIMIT;
		if (key == "cgi")
			return CONF_BLOCK_CGI;
		if (key == "error_page")
			return CONF_BLOCK_ERROR_PAGE;
		if (key == "index")
			return CONF_BLOCK_INDEX;
		if (key == "location")
			return CONF_SERVER_LOCATION;
		if (key == "listen")
			return CONF_SERVER_LISTEN;
		if (key == "redirect")
			return CONF_BLOCK_REDIRECT;
		if (key == "root")
			return CONF_BLOCK_ROOT;
		if (key == "server_name")
			return CONF_SERVER_NAME;
		return CONF_NOT_FOUND_TOKEN;
	}

	inline void	_skip_whitespaces(std::string *s) {
		while ((*s)[0] == '\t' || (*s)[0] == ' ')
			s->erase(0, 1);
	}

	bool _is_digits(const std::string &s) {
		return s.find_first_not_of("0123456789") == std::string::npos;
	}

	void _extract_value(const std::string &key, std::string *bucket,
		bool inside_location_block) {
		bucket->erase(0, key.size());
		_skip_whitespaces(bucket);

		char	last_chr = (*bucket)[bucket->size() - 1];
		if (last_chr != ';' && (inside_location_block && last_chr != '{')) {
			invalid_delimiter_error(*bucket);
			throw std::runtime_error("invalid delimiter");
		}
		bucket->erase(bucket->size() - 1, 1);
		if (bucket->find_first_of("\t-,;") != std::string::npos) {
			invalid_delimiter_error(*bucket);
			throw std::runtime_error("invalid delimiter");
		}
	}

	std::vector<std::string> *_split_string(const std::string &ref,
		char delimiter, std::vector<std::string> *bucket) {
		std::string s;
		std::istringstream st(ref);

		while (std::getline(st, s, delimiter))
			bucket->push_back(s);
		return bucket;
	}

	bool	_parse_configuration() {
		std::string line;
		std::stringstream	ss(_conf_file);

		std::vector<ILocation *>	locations;
		int token = 0, line_nbr = 0, scope = 0;
		while (std::getline(ss, line) && ++line_nbr) {
			token = _resolve_line(&line);
			switch (token) {
				case CONF_BLOCK_ALLOWED_METHODS: {
					_extract_value("allowed_methods", &line, false);
					std::vector<std::string> split;
					_split_string(line, ' ', &split);
					std::vector<std::string>::const_iterator it = split.begin();
					for (; it != split.end(); it++) {
						if (Models::get_method(*it) == Models::METHOD_UNKNOWN)
							return unknown_method_error(*it);
						if (locations.size() == 0)
							_servers.back()->set_method(Models::get_method(*it), true);
						else
							locations.back()->set_method(Models::get_method(*it), true);
					}
					continue;
				}
				case CONF_BLOCK_AUTOINDEX: {
					_extract_value("autoindex", &line, false);
					if (line != "on" && line != "off")
						return invalid_value_error(line, line_nbr);
					if (locations.size() == 0)
						_servers.back()->set_autoindex(line == "on");
					else
						locations.back()->set_autoindex(line == "on");
					continue;
				}
				case CONF_BLOCK_BODY_LIMIT: {
					_extract_value("body_limit", &line, false);
					if (!_is_digits(line))
						return invalid_value_error(line, line_nbr);
					if (locations.size() == 0)
						_servers.back()->set_body_limit(atoi(line.c_str()));
					else
						locations.back()->set_body_limit(atoi(line.c_str()));
					continue;
				}
				case CONF_BLOCK_CGI: {
					_extract_value("cgi", &line, false);
					if (line.find(" ") == std::string::npos)
						return invalid_value_error(line, line_nbr);
					std::string extension = line.substr(0, line.find(" "));
					std::string cgi_path = line.substr(line.find(" ") + 1, line.size());
					if (locations.size() == 0)
						_servers.back()->set_cgi(extension, cgi_path);
					else
						locations.back()->set_cgi(extension, cgi_path);
					continue;
				}
				case CONF_BLOCK_ERROR_PAGE: {
					_extract_value("error_page", &line, false);
					if (line.size() < 6 
						|| !_is_digits(line.substr(0, 3))
						|| line.find(" ") == std::string::npos 
						|| line.substr(0, line.find(" ")).size() != 3)
						return invalid_value_error(line, line_nbr);
					int error_code = atoi(line.substr(0, line.find(" ")).c_str());
					std::string page_path = line.substr(line.find(" ") + 1, line.size());
					if (locations.size() == 0)
						_servers.back()->set_error_page(error_code, page_path);
					else
						locations.back()->set_error_page(error_code, page_path);
				}
				case CONF_BLOCK_INDEX: {
					_extract_value("index", &line, false);

					std::vector<std::string> split;
					_split_string(line, ' ', &split);

					if (split.size() == 0)
						return invalid_value_error(line, line_nbr);
					std::vector<std::string>::const_iterator it = split.begin();
					for (; it != split.end(); it++) {
						if (it->size() > 0)
							_servers.back()->add_index(*it);
					}
					continue;
				}
				case CONF_SERVER_LOCATION: {
					++scope;
					_extract_value("location", &line, true);

					if (line.size() < 1 || line[0] != '/')
						return invalid_value_error(line, line_nbr);
					if (line[line.size() - 1] == ' ')
						line = line.substr(0, line.size() - 1);
					ILocation *block = _servers.back()->new_location(line);
					locations.push_back(block);
					continue;
				}
				case CONF_SERVER_LISTEN: {
					_extract_value("listen", &line, false);
					if (line.size() == 0)
						return invalid_value_error(line, line_nbr);
					if (line.find(":") == std::string::npos) {
						if (!_is_digits(line)) {
							_servers.back()->set_host(line);
							_servers.back()->set_port(80);
							continue;
						} else {
							_servers.back()->set_host("*");
							_servers.back()->set_port(atoi(line.c_str()));
							continue;
						}
					} else {
						if (line[0] == ':') {
							_servers.back()->set_host("*");
							_servers.back()->set_port(atoi(line.substr(1, line.size()).c_str()));
							continue;
						} else {
							_servers.back()->set_host(line.substr(0, line.find(":")));
							_servers.back()->set_port(atoi(line.substr(line.find(":") + 1, line.size()).c_str()));
							continue;
						}
					}
					return invalid_value_error(line, line_nbr);
				}
				case CONF_BLOCK_REDIRECT: {
					_extract_value("redirect", &line, false);

					if (line.find(" ") == std::string::npos)
						return invalid_value_error(line, line_nbr);

					int	status_code = atoi(line.c_str());
					if (status_code < 200 || status_code > 527)
						return invalid_http_code_error(status_code);

					std::string url = line.erase(0, 4);
					if (locations.size() == 0)
						_servers.back()->set_redirection(url, status_code);
					else
						locations.back()->set_redirection(url, status_code);
					continue;
				}
				case CONF_BLOCK_ROOT: {
					_extract_value("root", &line, false);
					if (line.size() == 0)
						return invalid_value_error(line, line_nbr);
					if (locations.size() == 0)
						_servers.back()->set_root(line);
					else
						locations.back()->set_root(line);
					continue;
				}
				case CONF_SERVER_NAME: {
					_extract_value("server_name", &line, false);

					if (line.size() == 0)
						return invalid_value_error(line, line_nbr);
					_servers.back()->set_name(line);
					continue;
				}
				case CONF_EMPTY_TOKEN:
					continue;
				case CONF_SERVER_OPENING: {
					++scope;
					_servers.push_back(new IServer());
					continue;
				}
				case CONF_ERRORENOUS_TOKEN:
					return errorneous_line_error(line, line_nbr);
				case CONF_BLOCK_CLOSING: {
					--scope;
					if (scope < 0)
						return invalid_scope_error(line, line_nbr);
					if (locations.size() > 0)
						locations.pop_back();
					continue;
				}
				default:
					return errorneous_line_error(line, line_nbr);
			}
		}
		return true;
	}
};
}  // namespace Conf
}  // namespace Webserv

#endif  // CONF_PARSER_HPP_
