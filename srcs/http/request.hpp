#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>
#include <iostream>

#include "../models/enums.hpp"

namespace Webserv {
namespace Http {
class Request {
	typedef Webserv::Models::EMethods		EMethod;

 private:
	EMethod	_method;
	std::string	_uri;
	float 	_http_version;

	std::map<std::string, std::string>	_headers;

 public:
	explicit Request(char *buffer) : _method(Models::METHOD_UNKNOWN), _uri(""),
		_http_version(0.0), _headers() {
		_parse(buffer);
		// std::cout << buffer << std::endl;
		// _method = Models::GET;
		// _http_version = 1.0;
		// _parse(buffer);
	}

 private:
	void	_parse(char *buffer) {
	}
};
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_REQUEST_HPP_
