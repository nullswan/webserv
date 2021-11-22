#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <stdlib.h>

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
	std::string _http_version;

	std::map<std::string, std::string>	_headers;

	bool	_closed;
 public:
	explicit Request(char *buffer) : _method(Models::METHOD_UNKNOWN), _uri(""),
		_http_version(""), _headers(), _closed(false) {
		_parse(buffer);

		// __repr__();
	}

	bool	bad_request() {
		std::cout << "Bad request" << std::endl;
		_closed = true;
		return false;
	}

	bool	closed() const {
		if (!_closed && _headers.find("Connection") != _headers.end()) {
			if (_headers.find("Connection")->second == "close")
				return true;
		}
		return _closed;
	}

	void	__repr__() {
		std::cout << "Request{method: " << Models::resolve_method(_method)
			<< ", _uri: " << _uri
			<< ", _http_version: " << _http_version << "}" << std::endl;

		std::map<std::string, std::string>::iterator it;
		for (it = _headers.begin(); it != _headers.end(); it++) {
			std::cout << "{" << it->first << ": " << it->second << "}" << std::endl;
		}
	}

 private:
	void	_parse(std::string buffer) {
		if (!_extract_method(buffer))
			return;
		if (!_extract_uri(buffer))
			return;
		if (!_extract_http_version(buffer))
			return;
		_extract_headers(buffer);
	}

	bool	_extract_method(std::string &buffer) {
		size_t	method_separator_pos = buffer.find(" ");
		if (method_separator_pos == std::string::npos)
			return bad_request();

		std::string	method_str = buffer.substr(0, method_separator_pos);
		_method = Models::get_method(method_str);
		buffer.erase(0, method_separator_pos + 1);
		return true;
	}

	bool	_extract_uri(std::string &buffer) {
		size_t	uri_separator_pos = buffer.find(" ");
		if (uri_separator_pos == std::string::npos)
			return bad_request();

		_uri = buffer.substr(0, uri_separator_pos);
		buffer.erase(0, uri_separator_pos + 1);
		return true;
	}

	bool	_extract_http_version(std::string &buffer) {
		size_t	version_separator_pos = buffer.find("\r\n");
		if (version_separator_pos == std::string::npos)
			return bad_request();

		_http_version = buffer.substr(0, version_separator_pos);
		buffer.erase(0, version_separator_pos + 2);
		return true;
	}

	void	_extract_headers(std::string &buffer) {
		size_t	header_separator_pos = buffer.find("\r\n");
		while (header_separator_pos != std::string::npos) {
			std::string	header_str = buffer.substr(0, header_separator_pos);
			size_t		header_name_separator_pos = header_str.find(":");
			if (header_name_separator_pos == std::string::npos)
				break;

			std::string	header_name = header_str.substr(0, header_name_separator_pos);
			std::string	header_value = header_str.substr(header_name_separator_pos + 2);
			_headers[header_name] = header_value;
			buffer.erase(0, header_separator_pos + 2);
			header_separator_pos = buffer.find("\r\n");
		}
	}
};
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_REQUEST_HPP_
