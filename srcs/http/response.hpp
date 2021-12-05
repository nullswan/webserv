#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <string.h>

#include <map>
#include <string>

#include "status.hpp"

namespace Webserv {
namespace Http {
class Response {
	typedef std::map<std::string, std::string> HeadersObject;

 private:
	std::string _body;
	std::string _payload;

	std::map<std::string, std::string> _headers;

	int _status;


 public:
	Response() : _body(""), _status(200) {
		_body = "Hello World!";
	}

	explicit Response(int code)
	:	_body(generate_status_page(code)),
		_status(code) {}

	bool	prepare() {
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
		char buf[30];
		std::time_t t = std::time(NULL);
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
