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

#include "conf/enums.hpp"
#include "conf/errors.hpp"
#include "http/enums.hpp"
#include "http/utils.hpp"
#include "models/IServer.hpp"
#include "models/ILocation.hpp"

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

	bool	run(int ac, char **av) {
		if (!_verify_arguments(ac, av))
			return false;
		try {
			if (!_parse_configuration())
				return false;
		} catch (std::exception &e) {
			return false;
		}
		if (_servers.size() <= 0)
			return no_server_error();
		_handle_vhosts();
		return _handle_interfaces();
	}

	void clear() {
		if (_servers.size() == 0)
			return;

		IServerList::iterator it = _servers.begin();
		for (; it != _servers.end(); it++) {
			delete *it;
		}
		_servers.clear();
	}

	IServerList	&get_servers() {
		return _servers;
	}

 private:
	bool		_handle_interfaces() {
		IServerList::const_iterator it = _servers.begin();
		for (; it != _servers.end(); it++) {
			IServerList::const_iterator it2 = it + 1;
			for (; it2 != _servers.end();  it2++) {
				if ((*it)->get_port() == (*it2)->get_port()) {
					return interface_rebind_error(
						(*it)->get_name(), (*it2)->get_name(),
						(*it)->get_port());
				}
			}
		}
		return true;
	}

	void	_handle_vhosts() {
		if (_servers.size() == 0)
			return;

		IServerList::iterator it = _servers.begin();
		for (; it != _servers.end(); it++) {
			IServerList::iterator it2 = it + 1;
			for (; it2 != _servers.end(); it2++) {
				if ((*it)->get_host() == (*it2)->get_host() &&
					(*it)->get_port() == (*it2)->get_port()) {
					(*it)->merge(*it2);
					delete *it2;
					_servers.erase(it2);
					it2--;
				}
			}
		}
	}

	bool	_default_configuration() {
		no_config_file_error();
		_conf_file = DEFAULT_CONF_FILE;
		std::cout << "[📄] using default configuration file" << std::endl;
		return true;
	}

	bool	_verify_arguments(int ac, char **av) {
		if (ac >= 3)
			return usage_error(av[0]);
		if (ac == 1)
			return _default_configuration();

		_conf_file_path = av[1];
		if (_conf_file_path.substr(
			_conf_file_path.size() - 5, _conf_file_path.size()) != ".conf")
			return config_file_extension_error(av[1]);

		return _dump_file(_conf_file_path, &_conf_file);
	}

	bool	_check_file_flags(const std::string &path) {
		struct stat buffer;
		if (stat(path.c_str(), &buffer) != 0)
			return file_not_found_error(path);
		else if (buffer.st_mode & S_IFDIR)
			return file_is_directory_error(path);

		if (access(path.c_str(), F_OK) == -1)
			return insufficient_permission_error(path);

		return true;
	}

	bool	_dump_file(const std::string &file, std::string *bucket) {
		std::ifstream	fs;

		if (!_check_file_flags(file))
			return false;
		fs.open(file.c_str());
		if (!fs || !fs.is_open())
			return cannot_open_error(_conf_file_path);

		fs.seekg(0, fs.end);
		int length = fs.tellg();
		fs.seekg(0, fs.beg);

		char *buffer = new char[length + 1];
		for (int i = 0; i < length + 1; i++)
			buffer[i] = 0;
		fs.read(buffer, length);

		fs.close();
		(*bucket) = buffer;
		delete[] buffer;
		return true;
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
		if (key == "upload_pass")
			return CONF_BLOCK_UPLOAD_PASS;
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
		if (bucket->find(';') != std::string::npos
		|| bucket->find(',') != std::string::npos) {
			illegal_char_error(*bucket);
			throw std::runtime_error("invalid char");
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
		Models::IBlock *current_block = NULL;

		int token = 0, line_nbr = 0, scope = 0;
		while (std::getline(ss, line) && ++line_nbr) {
			token = _resolve_line(&line);
			switch (token) {
				case CONF_BLOCK_ALLOWED_METHODS: {
					_extract_value("allowed_methods", &line, false);
					std::vector<std::string> split;
					_split_string(line, ' ', &split);
					current_block->deny_all_methods();
					std::vector<std::string>::const_iterator it = split.begin();
					for (; it != split.end(); it++) {
						HTTP::METHODS m = HTTP::enumerate_method(*it);
						if (m == HTTP::METH_UNKNOWN)
							return unknown_method_error(*it);
						if (m != HTTP::METH_GET && m != HTTP::METH_POST && m != HTTP::METH_DELETE)
							return not_implemented_method_error(*it);
						current_block->set_method(HTTP::enumerate_method(*it), true);
					}
					break;
				}
				case CONF_BLOCK_AUTOINDEX: {
					_extract_value("autoindex", &line, false);
					if (line != "on" && line != "off")
						return invalid_value_error(line, line_nbr);
					current_block->set_autoindex(line == "on");
					break;
				}
				case CONF_BLOCK_BODY_LIMIT: {
					_extract_value("body_limit", &line, false);
					if (!_is_digits(line))
						return invalid_value_error(line, line_nbr);
					current_block->set_body_limit(atoi(line.c_str()));
					break;
				}
				case CONF_BLOCK_CGI: {
					_extract_value("cgi", &line, false);
					if (line.find(" ") == std::string::npos)
						return invalid_value_error(line, line_nbr);
					std::string extension = line.substr(0, line.find(" "));
					std::string cgi_path = line.substr(line.find(" ") + 1, line.size());
					current_block->set_cgi(extension, cgi_path);
					break;
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
					std::string source;
					if (!_dump_file(page_path, &source))
						return invalid_value_error(page_path, line_nbr);
					current_block->set_error_page(error_code, source);
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
							current_block->add_index(*it);
					}
					continue;
				}
				case CONF_SERVER_LOCATION: {
					if (scope == 2)
						return nested_locations_error(line, line_nbr);
					++scope;
					_extract_value("location", &line, true);

					if (line.size() < 1 || line[0] != '/')
						return invalid_value_error(line, line_nbr);
					if (line[line.size() - 1] == ' ')
						line = line.substr(0, line.size() - 1);
					ILocation *block = _servers.back()->new_location(line);
					if (!block)
						return invalid_value_error(line, line_nbr);
					current_block = block;
					continue;
				}
				case CONF_SERVER_LISTEN: {
					_extract_value("listen", &line, false);
					if (scope != 1)
						return unexpected_token_line_error("listen", line_nbr);
					if (line.size() == 0)
						return invalid_value_error(line, line_nbr);

					if (line.find(":") == std::string::npos) {
						if (!_is_digits(line)) {
							if (line == "*")
								_servers.back()->set_host("0.0.0.0");
							else
								_servers.back()->set_host(line);
						} else {
							_servers.back()->set_host("0.0.0.0");
							_servers.back()->set_port(atoi(line.c_str()));
						}
						break;
					} else {
						if (line[0] == ':') {
							_servers.back()->set_host("0.0.0.0");
							_servers.back()->set_port(atoi(line.substr(1, line.size()).c_str()));
						} else {
							if (line.substr(0, line.find(":")) == "*")
								_servers.back()->set_host("0.0.0.0");
							else
								_servers.back()->set_host(line.substr(0, line.find(":")));
							_servers.back()->set_port(atoi(line.substr(
								line.find(":") + 1, line.size()).c_str()));
						}
						break;
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
					current_block->set_redirection(url, status_code);
					break;
				}
				case CONF_BLOCK_ROOT: {
					_extract_value("root", &line, false);
					if (line.size() == 0)
						return invalid_value_error(line, line_nbr);
					current_block->set_root(line);
					break;
				}
				case CONF_SERVER_NAME: {
					_extract_value("server_name", &line, false);
					if (scope != 1)
						return unexpected_token_line_error("server_name", line_nbr);
					if (line.size() == 0)
						return invalid_value_error(line, line_nbr);
					_servers.back()->set_name(line);
					break;
				}
				case CONF_BLOCK_UPLOAD_PASS: {
					_extract_value("upload_pass", &line, false);

					if (line.size() == 0)
						return invalid_value_error(line, line_nbr);
					current_block->set_upload_pass(line);
					break;
				}
				case CONF_EMPTY_TOKEN:
					break;
				case CONF_SERVER_OPENING: {
					if (scope != 0)
						return unexpected_token_line_error("server", line_nbr);
					++scope;
					_servers.push_back(new IServer());
					current_block = _servers.back();
					break;
				}
				case CONF_ERRORENOUS_TOKEN:
					return errorneous_line_error(line, line_nbr);
				case CONF_BLOCK_CLOSING: {
					if (scope < 0)
						return invalid_scope_error(line, line_nbr);
					if (scope == 2)
						current_block = _servers.back();
					if (scope == 1)
						current_block = 0;
					--scope;
					break;
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
