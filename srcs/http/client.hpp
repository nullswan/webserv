#ifndef HTTP_CLIENT_HPP_
#define HTTP_CLIENT_HPP_

#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <string>

#include "request.hpp"
#include "../models/enums.hpp"
#include "../models/consts.hpp"

namespace Webserv {
namespace Http {
class Client {
	typedef Webserv::Models::ERead		ERead;
	typedef Webserv::Http::Request	Request;

 private:
	struct sockaddr _addr;
	socklen_t _addr_len;

	int		_fd;
	bool	_close;

	Request	*_last_request;

 public:
	/*
		Always close client for the moment.
	*/
	explicit Client(int ev_fd)
		: _addr(), _addr_len(0), _fd(-1), _close(true), _last_request(0) {
		_fd = accept(ev_fd, &_addr, &_addr_len);
		if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
			close(_fd);
			_fd = -1;
			std::cerr << "fcntl() failed" << std::endl;
		}
	}

	~Client() {
		if (_fd != -1)
			close(_fd);
	}

	ERead read_request() {
		char buffer[REQ_BUF_SIZE];
		int n = recv(_fd, buffer, REQ_BUF_SIZE, 0);
		if (n == -1) {
			return Models::READ_ERROR;
		} else if (n == 0) {
			return Models::READ_EOF;
		} else {
			if (_last_request)
				delete _last_request;
			_last_request = new Request(buffer);
			return Models::READ_OK;
		}
	}

	void	send_response() {
		std::string head = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-length: 12\r\n\r\nHello World!";

		send(_fd, head.c_str(), head.size(), 0);

		if (_last_request) {
			delete _last_request;
			_last_request = 0;
		}
	}

	int	get_fd() const { return _fd; }
	bool do_close() const { return _close; }
};
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_CLIENT_HPP_
