#ifndef SERVER_CGI_HPP_
#define SERVER_CGI_HPP_

#include <sys/wait.h>
#include <sys/types.h>

#include <map>
#include <string>
#include <utility>

#include "http/request.hpp"
#include "http/response.hpp"

namespace Webserv {
namespace Server {

class CGI {
 public:
	typedef std::multimap<std::string, std::string> Headers;
	typedef std::pair<std::string, std::string>		HeaderPair;

 private:
	const std::string	_bin_path;
	const std::string	_file_path;
	const std::string	_query_string;

	HTTP::METHODS		_method;

	std::string	_body;
	Headers		_headers;

	int		_link[2];
	pid_t	_pid;

 public:
	CGI(const std::string &bin_path,
		const std::string &file_path, const std::string &query,
		const HTTP::METHODS &method)
	:	_bin_path(bin_path),
		_file_path(file_path),
		_query_string(query),
		_method(method) {}


	bool	setup() {
		if (pipe(_link) == -1)
			return false;
		(void)_query_string;
		return true;
	}

	bool		run() {
		pid_t	_wait;

		_pid = fork();
		if (_pid < 0) {
			return false;
		} else if (_pid == 0) {
			dup2(_link[STDOUT_FILENO], STDOUT_FILENO);
			close(_link[STDIN_FILENO]);
			close(_link[STDOUT_FILENO]);
			char	*argv[] = {
				(char *)_bin_path.c_str(), (char *)_file_path.c_str(), NULL};
			if (execve(argv[0], argv, NULL) == -1)
				exit(EXIT_FAILURE);
			exit(0);
		} else {
			waitpid(_pid, &_wait, 0);
			close(_link[STDOUT_FILENO]);

			return _extract_response();
		}
		return true;
	}

	const std::string &get_output() const {
		return _body;
	}
	const Headers &get_headers() const {
		return _headers;
	}

 private:	
	bool	_extract_response() {
		char	buf[4096];
		while (read(_link[0], buf, 4096) > 0)
			_body += std::string(buf);
		return _parse_response_headers();
	}

	bool	_parse_response_headers() {
		std::string headers = _body.substr(0, _body.find("\r\n\r\n"));
		_body = _body.substr(_body.find("\r\n\r\n") + 4);
		// do {
		// 	std::string key = headers.substr(0, headers.find(":"));
		// 	std::string value = headers.substr(headers.find(":") + 2, headers.find("\r\n"));
		// 	_headers.insert(HeaderPair(key, value));
		// 	headers = headers.substr(headers.find("\r\n") + 2);
		// } while (headers.find("\n") != std::string::npos);
		return true;
	}
};

}  // namespace Server
}  // namespace Webserv

#endif  // SERVER_CGI_HPP_
