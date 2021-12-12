#ifndef SERVER_POLL_HPP_
#define SERVER_POLL_HPP_

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <map>
#include <string>
#include <vector>
#include <csignal>
#include <stdexcept>
#include <iostream>

#include "consts.hpp"
#include "http/enums.hpp"
#include "http/codes.hpp"
#include "http/client.hpp"
#include "models/IServer.hpp"
#include "server/instance.hpp"

namespace Webserv {
namespace Server {
class Poll {
 public:
	typedef Webserv::Models::IServer	IServer;

	typedef std::map<int, Instance *>		InstanceObject;
	typedef std::map<int, HTTP::Client *> 	ClientObject;

 private:
	bool	_alive;
	int		epoll_fd;

	InstanceObject	_instances;
	ClientObject	_clients;

 public:
	Poll()
	:	_alive(true), epoll_fd(-1) {
		#ifdef WEBSERV_BENCHMARK
		std::cout << "[🚀] starting in benchmark mode" << std::endl;
		#endif
		#ifdef WEBSERV_SESSION
		std::cout << "[🔑] using session module" << std::endl;
		#endif
		HTTP::init_status_map();
	}

	~Poll() {
		for (InstanceObject::iterator it = _instances.begin();
			it != _instances.end(); ++it)
			delete it->second;

		for (ClientObject::iterator it = _clients.begin(); it != _clients.end(); ++it)
			delete it->second;
		close(epoll_fd);
	}

	void	init(const std::vector<IServer *> &servers) {
		if (!_create_poll())
			throw std::runtime_error("Error while initializing epoll.");
		if (!_add_servers(servers))
			throw std::runtime_error("Error while adding servers to epoll.");
		if (!_add_stdin())
			throw std::runtime_error("Error while adding stdin to epoll.");
	}

	int	run() {
		struct epoll_event events[WEBSERV_MAX_CONNS];
		int i, evs = 0;
		std::cout << "[📭] up and awaiting..." << std::endl;
		while (_alive) {
			int nfds = epoll_wait(epoll_fd, events, WEBSERV_MAX_CONNS, 1000);
			for (i = 0; i < nfds; ++i) {
				++evs;
				int ev_fd = events[i].data.fd;
				if (ev_fd == STDIN_FILENO) {
					_handle_stdin();
					continue;
				}
				if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP) {
					_handle_aborted(ev_fd);
					continue;
				}
				if (events[i].events & EPOLLIN) {
					std::map<int, Instance *>::iterator it = _instances.find(ev_fd);
					if (it != _instances.end())
						_handle_connection(it->second, ev_fd);
					else
						_handle_read(ev_fd);
				} else if (events[i].events & EPOLLOUT) {
					_handle_write(ev_fd);
				}
			}
			if (nfds == 0 || evs > 500)
				_garbage_collector(&evs);
		}

		return 0;
	}

 private:
	void	_garbage_collector(int *evs) {
		#ifdef WEBSERV_SESSION
		_collect_expired_sessions();
		#endif
		_handle_expired_clients();
		*evs = 0;
	}

	bool	_create_poll() {
		epoll_fd = epoll_create1(0);
		return (epoll_fd != -1);
	}

	bool	_add_servers(const std::vector<IServer *> &servers) {
		std::vector<IServer *>::const_iterator it = servers.begin();
		for (; it != servers.end(); it++) {
			if (!_add_server(*it)) {
				std::cout << "unable to add " << (*it)->get_name()
				<< " (" << (*it)->get_host() << ":" << (*it)->get_port() << ")"
				<< std::endl;
				return false;
			}
		}
		return true;
	}
	bool	_add_server(const IServer* serv) {
		Instance *new_server;
		try {
			new_server = new Instance(*serv);
			if (!new_server) {
				std::cerr << "add_server: alloc failed" << std::endl;
				return false;
			}
		} catch (std::exception &e) {
			return false;
		}

		int new_fd = new_server->get_fd();
		if (new_fd == -1) {
			delete new_server;
			std::cerr << "add_server: invalid fd" << std::endl;
			return false;
		}

		struct epoll_event event = {};
		event.events = EPOLLIN;
		event.data.fd = new_fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1) {
			delete new_server;
			std::cerr << "add_server: epoll_ctl failed" << std::endl;
			return false;
		}

		_instances[new_fd] = new_server;
		return true;
	}
	bool	_add_stdin() {
		struct	epoll_event event = {};
		event.events = EPOLLIN;
		event.data.fd = STDIN_FILENO;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event) == -1) {
			std::cerr << "add_stdin: epoll_ctl failed" << std::endl;
			return false;
		}
		return true;
	}

	void	_handle_connection(IServer *master, int fd) {
		HTTP::Client *client = new HTTP::Client(master, fd);
		if (!client) {
			std::cerr << "handle_connection: alloc failed" << std::endl;
			return;
		}

		int new_fd = client->get_fd();
		if (new_fd == -1) {
			delete client;
			std::cerr << "handle_connection: invalid fd" << std::endl;
			return;
		}

		struct epoll_event event = {};
		event.events = EPOLLIN;
		event.data.fd = new_fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1) {
			delete client;
			std::cerr << "handle_connection: epoll_add failed()" << std::endl;
			return;
		}
		_clients[new_fd] = client;
	}
	void	_handle_read(int ev_fd) {
		if (_clients.find(ev_fd) == _clients.end()) {
			std::cerr << "handle_read: invalid fd" << std::endl;
			return;
		}

		HTTP::Client *client = _clients[ev_fd];
		HTTP::READ ret = client->read_request();
		if (ret == HTTP::READ_EOF || ret == HTTP::READ_ERROR)
			return _delete_client(ev_fd, client);
		else if (ret == HTTP::READ_OK)
			return _change_epoll_state(ev_fd, EPOLLOUT);
	}
	void	_handle_write(int ev_fd) {
		HTTP::Client *client = _clients[ev_fd];
		if (!client) {
			std::cerr << "handle_write: invalid fd" << std::endl;
			return;
		}

		if (client->send_response())
			return _delete_client(ev_fd, client);
		return _change_epoll_state(ev_fd, EPOLLIN);
	}

	void	_handle_stdin() {
		std::string line;

		#ifndef WEBSERV_BENCHMARK
			if (!std::getline(std::cin, line) || line == "quit" || line == "exit") {
				_alive = false;
				std::cout << "[📪] shutting down..." << std::endl;
			}
		#else
			std::getline(std::cin, line);
			if (line == "quit" || line == "exit") {
				_alive = false;
				std::cout << "[📪] shutting down..." << std::endl;
			}
		#endif
	}

	void	_handle_aborted(int ev_fd) {
		if (_clients.find(ev_fd) != _clients.end())
			_delete_client(ev_fd, _clients[ev_fd]);
		else
			close(ev_fd);
	}

	void	_handle_expired_clients() {
		struct timeval now;
		gettimeofday(&now, NULL);

		ClientObject::iterator it = _clients.begin();
		for (; it != _clients.end(); it++) {
			if (it->second->is_expired(now.tv_sec)) {
				it->second->abort(408);
				_delete_client(it->first, it->second);
				return _handle_expired_clients();
			}
		}
	}

	#ifdef WEBSERV_SESSION
	void	_collect_expired_sessions() const {
		InstanceObject::const_iterator it = _instances.begin();
		for (; it != _instances.end(); it++)
			it->second->collect_sessions();
	}
	#endif

	void	_delete_client(int ev_fd, HTTP::Client *client) {
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ev_fd, NULL);
		delete client;
		_clients.erase(ev_fd);
	}

	void	_change_epoll_state(int ev_fd, int state) {
		struct epoll_event event = {};
		event.events = state;
		event.data.fd = ev_fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ev_fd, &event) == -1) {
			std::cerr << "_change_epoll_state: failed()" << std::endl;
			HTTP::Client *client = _clients[ev_fd];
			return _delete_client(ev_fd, client);
		}
	}
};
}  // namespace Server
}  // namespace Webserv

#endif  // SERVER_POLL_HPP_
