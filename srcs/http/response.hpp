#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <map>
#include <vector>
#include <string>

#include "codes.hpp"
#include "request.hpp"
#include "../models/IServer.hpp"

namespace Webserv {
namespace HTTP {
class Response {
	typedef Webserv::Models::IServer 				IServer;
	typedef std::map<std::string, std::string>		HeadersObject;

 private:
	std::string _body;
	std::string _payload;

	HeadersObject _headers;

	int _status;

	Request *_req;

 public:
	explicit Response(Request *request)
	:	_status(request->get_code()), _req(request) {}

	explicit Response(int code)
	:	_status(code), _req(0) {}

	bool	prepare(IServer *master) {
		if (_req && _status < 400) { invoke(master); }
		if (_status >= 400) {
			if (_req) {
				_body = master->get_error_page(_status, _req->get_host(), _req->get_uri());
				if (_body == "")
					_body = generate_status_page(_status);
			} else {
				_body = generate_status_page(_status);
			}
		}
		_payload = _prepare_headers() + _body + "\r\n";
		return true;
	}

	int		status() const { return _status; }
	void	set_status(int status) { _status = status; }
	const void *toString() const { return _payload.c_str(); }
	size_t	size() const { return _payload.size(); }

 private:
	void	invoke(IServer *master) {
		const Models::ILocation	*loc = master->get_location_using_vhosts(
			_req->get_host(), _req->get_uri());

		if (_req->get_method() == METH_GET) {
			std::string cgi_path;
			if (loc)
				cgi_path = loc->get_cgi(_req->get_uri());
			else
				cgi_path = master->get_cgi(_req->get_uri());
			if (cgi_path != "")
				std::cout << "[deprecated] appears to be a cgi path" << std::endl;
				// return _handle_cgi();

			struct stat db;
			std::string real_path;
			if (!loc)
				real_path = master->get_root();
			else
				real_path = loc->get_root();
			real_path += _req->get_uri();
			errno = 0;
			if (stat(real_path.c_str(), &db) == -1) {
				_status = 500;
				if (errno == ENOENT || errno == ENOTDIR)
					_status = 404;
				return;
			}

			switch (db.st_mode & S_IFMT) {
				case S_IFDIR: {
					errno = 0;

					DIR*			dir_ptr = opendir(real_path.c_str());
					if (dir_ptr == NULL) {
						_status = 500;
						if (errno == EACCES)
							_status = 403;
						return;
					}

					errno = 0;
					struct dirent	*file;
					std::vector<struct dirent> files;
					while ((file = readdir(dir_ptr))) {
						if (errno) {
							_status = 500;
							closedir(dir_ptr);
							return;
						}
						files.push_back(*file);
					}
					closedir(dir_ptr);

					Models::IBlock::IndexObject indexs_addr;
					if (loc)
						indexs_addr = loc->get_indexs();
					else
						indexs_addr = master->get_indexs();

					// attempt index
					Models::IBlock::IndexObject::const_iterator it;
					for (it = indexs_addr.begin(); it != indexs_addr.end(); it++) {
						std::vector<struct dirent>::iterator	it_file;
						for (it_file = files.begin(); it_file != files.end(); it_file++) {
							if (it_file->d_name == *it) {
								const std::string index_path = real_path + "/" + *it;
								int fd = open(index_path.c_str(), O_RDONLY | O_NONBLOCK);
								if (fd == -1) {
									_status = 500;
									if (errno == EACCES)
										_status = 403;
								} else {
									_body = get_file_content(fd);
									_status = 200;
								}
								return;
							}
						}
					}
					// if allowed, attempt autoindex
					// otherwise pass file vector to start autoindex
					break;
				}
				default:
					errno = 0;

					int fd = open(real_path.c_str(), O_RDONLY | O_NONBLOCK);
					if (fd == -1) {
						_status = 500;
						if (errno == EACCES)
							_status = 403;
					} else {
						_body = get_file_content(fd);
						_status = 200;
					}
					return;
			}
			_status = 404;
		}
		// else if (_req->get_method() == METH_POST)
		// else if (_req->get_method() == METH_DELETE)
		// else
			// prob stand for request not allowed
			// ie not authorized
			// else 501
	}

	std::string _prepare_headers() {
		_headers["Connection"] = "keep-alive";
		_headers["Content-Type"] = "text/html; charset=utf-8";
		_headers["Content-Length"] = _toString(_body.size());
		_set_header_date();
		_headers["Server"] = WEBSERV_SERVER_VERSION;
		#ifdef WEBSERV_BUILD_COMMIT
			_headers["Server"] += WEBSERV_BUILD_COMMIT;
		#endif

		std::stringstream head;
		head << "HTTP/1.1 " << _status << " " << resolve_code(_status) << "\r\n";

		std::string headers;
		HeadersObject::iterator it = _headers.begin();
		for (; it != _headers.end(); ++it) {
			headers += it->first + ": " + it->second + "\r\n";
		}

		return head.str() + headers + "\r\n";
	}

	void	_set_header_date() {
		std::time_t t = std::time(NULL);
		char buf[30];

		strftime(buf, sizeof(buf), "%a, %d %b %Y %T %Z", std::localtime(&t));
		_headers["Date"] = std::string(buf);
	}

	static std::string _toString(size_t size) {
		std::stringstream ss;
		ss << size;
		return ss.str();
	}

	std::string get_file_content(int fd) {
		std::stringstream ss;
		char buf[1024];
		ssize_t n;
		while ((n = read(fd, buf, sizeof(buf))) > 0) {
			ss.write(buf, n);
		}
		close(fd);
		return ss.str();
	}
};
}  // namespace HTTP
}  // namespace Webserv

#endif  // HTTP_RESPONSE_HPP_
