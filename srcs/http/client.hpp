#ifndef HTTP_CLIENT_HPP_
#define HTTP_CLIENT_HPP_

#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <ctime>
#include <string>
#include <sstream>

#include "request.hpp"
#include "response.hpp"
#include "../models/IServer.hpp"
#include "../models/enums.hpp"
#include "../models/consts.hpp"

namespace Webserv {
namespace Http {
class Client {
	typedef Webserv::Models::EMethods	EMethods;
	typedef Webserv::Models::ERead	ERead;
	typedef Webserv::Http::Request	Request;
	typedef Webserv::Models::IServer	IServer;

 private:
	IServer	*_master;

	struct sockaddr_in	_addr;
	socklen_t 			_addr_len;

	std::string 	_ip;
	int				_fd;
	struct timeval 	_last_ping;

	Request		*_last_request;
	Response	*_last_response;

 public:
	Client(IServer *master, int ev_fd)
		: _master(master), _addr(), _addr_len(0), _fd(-1) ,
		_last_request(0), _last_response(0) {
		_fd = accept(ev_fd, (struct sockaddr *)&_addr, &_addr_len);
		if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
			close(_fd);
			_fd = -1;
			std::cerr << "fcntl() failed" << std::endl;
		}
		gettimeofday(&_last_ping, NULL);
		#ifndef WEBSERV_BENCHMARK
			_resolve_client_ip();
		#endif
	}

	~Client() {
		if (_fd != -1)
			close(_fd);
		if (_last_request)
			delete _last_request;
		if (_last_response)
			delete _last_response;
	}

	ERead read_request() {
		char buffer[REQ_BUF_SIZE + 1] = {0};
		int n = recv(_fd, buffer, REQ_BUF_SIZE, 0);
		if (n == -1) {
			return Models::READ_ERROR;
		} else if (n == 0) {
			return Models::READ_EOF;
		} else {
			if (_last_request == NULL) {
				_last_request = new Request(buffer);
				_last_ping = *(_last_request->get_time());
			}
			else {
				_last_request->handle_buffer(buffer);
			}
			return _request_status();
		}
	}

	bool	send_response() {
		if (_last_response)
			delete _last_response;
		_last_response = new Response();
		_last_response->prepare();
		send(_fd, _last_response->toString(), _last_response->size(), 0);
		return _close();
	}

	int	get_fd() const { return _fd; }
	bool	is_expired(time_t now) const {
		return (now - _last_ping.tv_sec) > TIMEOUT;
	}

 private:
	ERead	_request_status() {
		bool header_status = _last_request->get_header_status();
		const std::string request = _last_request->get_raw_request();
		if (header_status == false && request.find("\r\n\r\n") == std::string::npos) {
			return Models::READ_WAIT;
		}
		else {
			if (header_status == false) {
				_last_request->init();
				_last_request->set_header_status(true);
			}
			const EMethods method = _last_request->get_method();
			if (method == Models::POST) {
				_last_request->read_body();
				const bool body_status = _last_request->get_body_status();
				if (body_status == false) {
					return Models::READ_WAIT;
				}
				_last_request->__repr__();
				return Models::READ_OK;
			}
			return Models::READ_OK;
		}
	}

	bool	_close() {
		if (_last_request) {
			bool state = _last_request->closed();
			#ifndef WEBSERV_BENCHMARK
				__log();
			#endif
			delete _last_request;
			_last_request = NULL;
			return state;
		}
		return true;
	}

	void	__log() const {
		struct timeval _end;
		gettimeofday(&_end, NULL);

		time_t time = (time_t)_end.tv_sec;
		struct tm local_time;
		localtime_r(&time, &local_time);
		char buffer[25];

		strftime(buffer, 25, "%Y/%m/%d - %H:%M:%S", &local_time);
		std::cout << "[\033[1;36mWEBSERV\033[0m] " << buffer << " |"
		<< get_method() << " " << get_uri() << " |"
		<< get_http_code() << "| "
		<< get_time_diff(&_end) << " | "
		<< get_client_ip() << " -> " << get_master_ip() << std::endl;
	}

	void	_resolve_client_ip() {
		getpeername(_fd, (struct sockaddr *)&_addr, &_addr_len);
		_ip = inet_ntoa(_addr.sin_addr);
	}

	std::string	get_client_ip() const {
		return _ip;
	}

	std::string get_master_ip() const {
		return _master->get_ip();
	}

	std::string	get_http_code() const {
		if (_last_response)
			return Models::resolve_decorated_http_code(_last_response->get_http_code());
		return "?";
	}

	std::string	get_method() const {
		if (_last_request)
			return Models::resolve_decorated_method(_last_request->get_method());
		return Models::resolve_decorated_method(Models::METHOD_UNKNOWN);
	}

	std::string get_uri() const {
		if (_last_request)
			return _last_request->get_uri();
		return "?";
	}

	const struct timeval *get_time() const {
		if (_last_request)
			return _last_request->get_time();
		return NULL;
	}

	std::string	get_time_diff(struct timeval *ptr) const {
		if (get_time() == 0)
			return "-";
		size_t usec = ptr->tv_usec - get_time()->tv_usec;
		size_t sec = ptr->tv_sec - get_time()->tv_sec;
		std::stringstream ss;

		if (sec > 0) {
			ss << sec << " s";
		} else {
			if (usec >= 1000)
				ss << usec / 1000 << " ms";
			else
				ss << usec << " μs";
		}

		std::string str = ss.str();
		return str.insert(0, 9 - str.size(), ' ');
	}
};
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_CLIENT_HPP_
