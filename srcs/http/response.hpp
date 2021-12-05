#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <string.h>

#include <string>

namespace Webserv {
namespace Http {
class Response {
 private:
	std::string _header;
	std::string _body;
	std::string _payload;

	std::map<std::string, std::string> _headers;
 public:
	Response() : _header(""), _body("") {
		_headers["Server"] = WEBSERV_SERVER_VERSION;
		#ifdef WEBSERV_BUILD_COMMIT
			_headers["Server"] += WEBSERV_BUILD_COMMIT;
		#endif

		// Date: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Date
		// Content-Length: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Type
		// Content-Type: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Type

		_header = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-length: 12\r\n\r\n";

		_body = "Hello World!";
	}

	size_t size() const { return _payload.size(); }
	bool	prepare() {
		_payload = _header + _body;
		return true;
	}
	const void * toString() const {
		return _payload.c_str();
	}

	std::string	get_http_code() {
		return "404";
	}
};
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_RESPONSE_HPP_
