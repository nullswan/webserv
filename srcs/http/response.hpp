#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <map>
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

			struct stat db;  // refer to man stat
			std::string real_path;
			if (!loc)
				real_path = master->get_root();
			else
				real_path = loc->get_root();
			real_path += _req->get_uri();
			if (stat(real_path.c_str(), &db) == -1) {
				_status = 404;
				return;
			}

			switch (db.st_mode & S_IFMT) {
				case S_IFDIR: {
					std::cout << "is_dir" << std::endl;
					// attempt index
					// attempt autoindex
					break;
				}
				case S_IFLNK: {
					std::cout << "is_symlink" << std::endl;
					// resolve
					break;
				}
				case S_IFREG: {
					std::cout << "is_file" << std::endl;
					// stream
					break;
				}
				default:
					_status = 500;
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
};
}  // namespace HTTP
}  // namespace Webserv

#endif  // HTTP_RESPONSE_HPP_
