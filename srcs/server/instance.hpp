/*

	Represent a server instance.
		-> Defined as a server block in the configuration file.

*/

#ifndef SERVER_INSTANCE_HPP_
#define SERVER_INSTANCE_HPP_

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>	 // inet_addr
#include <netinet/in.h>	 // sockaddr_in
#include <sys/socket.h>

#include <string>

#include "../models/consts.hpp"
#include "../models/IServer.hpp"

namespace Webserv {
namespace Server {
class Instance : protected Webserv::Models::IServer {
 public:
	typedef Webserv::Models::IServer IServer;
	typedef Webserv::Models::ILocation ILocation;

 private:
	int	_fd;

 public:
	explicit Instance(const IServer &serv)
		: IServer(serv), _fd(-1) {
		_setup();
	}

	~Instance() {
		if (_fd != -1)
			close(_fd);
	}

	int get_fd() const { return _fd; }

 private:
	void	_setup() {
		_create_socket();
		_set_noblock();
		_set_sockopt();
		_bind_socket();
		_listen_socket();
	}

	void	_create_socket() {
		_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_fd == -1)
			throw std::runtime_error("socket() failed");
	}
	void	_set_noblock() {
		if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("fcntl() failed");
	}
	void	_set_sockopt() {
		int _true = 1;
		if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_true, sizeof(_true)) == -1)
			throw std::runtime_error("setsockopt() failed");
	}
	void	_bind_socket() {
		struct sockaddr_in addr;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(_port);
		addr.sin_addr.s_addr = inet_addr(_host.c_str());

		if (bind(_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
			throw std::runtime_error("bind() failed");
	}
	void	_listen_socket() {
		if (listen(_fd, MAX_CONNS) == -1)
			throw std::runtime_error("listen() failed");
	}
};
}  // namespace Server
}  // namespace Webserv

#endif  // SERVER_INSTANCE_HPP_
