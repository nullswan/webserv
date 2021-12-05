#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <string.h>

#include <map>
#include <string>

#include "status.hpp"
#include "request.hpp"
#include "../models/IServer.hpp"

namespace Webserv {
namespace Http {
class Response {
	typedef Webserv::Models::IServer 				IServer;
	typedef std::map<std::string, std::string>	HeadersObject;

 private:
	std::string _body;
	std::string _payload;

	std::map<std::string, std::string> _headers;

	int _status;

	Request *_req;

 public:
	explicit Response(Request *request)
	:	_status(request->get_code()), _req(request) {}

	explicit Response(int code)
	:	_status(code) {}

	bool	prepare(IServer *master) {
		(void)master;
		if (_req) {
			// resolve
		}

		if (_status != 200) {
			if (_req) {
				_body = master->get_error_page(_status, _req->get_host(), _req->get_uri());
				if (_body == "")
					_body = generate_status_page(_status);
			} else {
				_body = generate_status_page(_status);
			}
		}

		_headers["Content-Type"] = "text/html; charset=utf-8";
		_headers["Content-Length"] = _toString(_body.size());
		_headers["Server"] = WEBSERV_SERVER_VERSION;
		#ifdef WEBSERV_BUILD_COMMIT
			_headers["Server"] += WEBSERV_BUILD_COMMIT;
		#endif
		_set_date();

		std::stringstream head;
		head << "HTTP/1.1 " << status() << " " << resolve_code(status()) << "\r\n";

		std::string headers;
		HeadersObject::iterator it = _headers.begin();
		for (; it != _headers.end(); ++it) {
			headers += it->first + ": " + it->second + "\r\n";
		}

		_payload = head.str() + headers + "\r\n" +  _body + "\r\n";
		return true;
	}

	int		status() const { return _status; }
	void	set_status(int status) { _status = status; }
	const void *toString() const { return _payload.c_str(); }
	size_t	size() const { return _payload.size(); }

 private:
	void	_set_date() {
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
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_RESPONSE_HPP_
