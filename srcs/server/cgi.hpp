#ifndef SERVER_CGI_HPP_
#define SERVER_CGI_HPP_

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <map>
#include <string>
#include <utility>

#include "http/request.hpp"
#include "http/utils.hpp"

namespace Webserv {
namespace Server {

class CGI {
 public:
	typedef std::multimap<std::string, std::string> Headers;
	typedef std::pair<std::string, std::string>		HeaderPair;
	typedef std::map<std::string, std::string> 		EnvVar;

 private:
	const std::string	_bin_path;
	const std::string	_file_path;
	const std::string	_out_file;

	EnvVar	_env;

	HTTP::METHODS	_method;

	std::string		_body;
	Headers			_headers;

	int		_fds[2];
	int		_out_fd;

 public:
	CGI(const std::string &bin_path,
		const std::string &file_path, const std::string &query,
		const HTTP::METHODS &method)
	:	_bin_path(bin_path),
		_file_path(file_path),
		_out_file(HTTP::rand_string(16)),
		_method(method) {
		_env["QUERY_STRING"] = query;
	}

	~CGI() {
		_delete_temp_file();
	}

	bool	setup(const std::string &request,
		const HTTP::Request::HeadersObject &headers) {
		if (pipe(_fds) == -1)
			return false;

		write(_fds[1], request.c_str(), request.size());
		_env["CONTENT_LENGTH"] = _toString(request.size());

		_out_fd = open(_out_file.c_str(), O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
		if (_out_fd == -1)
			return false;
		close(_fds[STDOUT_FILENO]);
		return setup_env(headers);
	}

	bool	setup_env(const HTTP::Request::HeadersObject &headers) {
		HTTP::Request::HeadersObject::const_iterator it = headers.begin();
		for (; it != headers.end(); ++it) {
			if (it->first == "content-type")
				_env["CONTENT_TYPE"] = it->second;
			_env["HTTP_" + _header_to_hcgi(it->first)] = it->second;
		}

		_env["GATEWAY_INTERFACE"] = "CGI/1.1";
		_env["SCRIPT_FILENAME"] = _file_path;
		_env["REDIRECT_STATUS"] = "200";

		if (_method == HTTP::METH_GET)
			_env["REQUEST_METHOD"] = "GET";
		else if (_method == HTTP::METH_POST)
			_env["REQUEST_METHOD"] = "POST";
		else
			_env["REQUEST_METHOD"] = "UNKNOWN";
		return true;
	}

	int	run() {
		bool	timeout = false;
		pid_t 	worker_pid = fork();

		if (worker_pid == 0)
			__worker_flow();

		pid_t timeout_pid = fork();
		if (timeout_pid == 0)
			__timer_flow();

		while (true) {
			pid_t exited_pid = wait(NULL);
			if (exited_pid == worker_pid) {
				kill(timeout_pid, SIGKILL);
				break;
			} else if (exited_pid == timeout_pid) {
				kill(worker_pid, SIGKILL);
				waitpid(worker_pid, 0, 0);
				close(_out_fd);
			}
		}
		if (timeout)
			waitpid(worker_pid, 0, 0);
		else
			waitpid(timeout_pid, 0, 0);
		close(_out_fd);
		if (timeout)
			return 2;
		return _extract_response();
	}

	const std::string &get_output() const { return _body; }
	const Headers &get_headers() const { return _headers; }

 private:
	void	__worker_flow() {
		dup2(_fds[STDIN_FILENO], STDIN_FILENO);
		dup2(_out_fd, STDOUT_FILENO);
		close(_fds[STDIN_FILENO]);

		char	*argv[] = {
			const_cast<char*>(_bin_path.c_str()),
			const_cast<char*>(_file_path.c_str()),
		NULL};
		if (execve(argv[0], argv, _dump_env()) == -1)
			exit(EXIT_FAILURE);
	}

	void	__timer_flow() {
		sleep(WEBSERV_CGI_TIMEOUT);
	}

	void	_delete_temp_file() { remove(_out_file.c_str()); }

	char	**_dump_env() {
		char **ret = reinterpret_cast<char**>(
				malloc(sizeof(char *) * (_env.size() + 1)));
		ret[_env.size()] = 0;

		EnvVar::const_iterator it = _env.begin();
		for (std::size_t i = 0; it != _env.end(); ++it, i++) {
			std::string payload = it->first + "=" + it->second;
			ret[i] = strdup(payload.c_str());
		}
		return ret;
	}

	bool	_extract_response() {
		std::ifstream	file(_out_file.c_str());
		if (!file.is_open())
			return false;
		std::string data((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
		file.close();
		return _parse_headers(data);
	}

	static inline std::string _header_to_hcgi(std::string in) {
		for (std::size_t i = 0; i < in.size(); i++) {
			if (in[i] >= 'a' && in[i] <= 'z')
				in[i] = in[i] - 32;
			else if (in[i] == '-')
				in[i] = '_';
		}
		return in;
	}

	bool	_parse_headers(std::string data) {
		std::size_t sep_pos = data.find("\r\n\r\n");
		if (sep_pos == std::string::npos)
			return false;

		std::string headers = data.substr(0, sep_pos);
		_body = data.substr(sep_pos + 4);

		while ((sep_pos = headers.find(": ")) != std::string::npos) {
			const std::string key = headers.substr(0, sep_pos);

			std::size_t nl_pos = 0;
			while (headers[nl_pos] && headers[nl_pos] != '\r')
				++nl_pos;
			std::string value = headers.substr(sep_pos + 2, nl_pos);

			if (value.find("\r") != std::string::npos)
				value = value.substr(0, value.find("\r"));

			_headers.insert(HeaderPair(key, value));
			if (key.size() + 2 + value.size() >= headers.size())
				break;
			headers = headers.substr(nl_pos + 2);
		}
		return true;
	}

	std::string	_toString(size_t num) {
		std::stringstream ss;

		ss << num;
		return ss.str();
	}
};

}  // namespace Server
}  // namespace Webserv

#endif  // SERVER_CGI_HPP_
