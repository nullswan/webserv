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
	struct timeval _time;

	EMethod		_method;
	std::string	_uri;
	std::string _http_version;

	std::map<std::string, std::string>	_headers;
	bool								_headers_done;

	bool	_closed;

 public:
	explicit Request(char *buffer) : _method(Models::METHOD_UNKNOWN), _uri(""),
		_http_version(""), _headers(), _headers_done(false), _closed(false) {
		gettimeofday(&_time, NULL);
		_parse(buffer);
	}

	void	parse_content(std::string buffer) {
		if (_headers_done == false)
			_extract_headers(&buffer);
		else
			_extract_body(&buffer);
	}

	bool	bad_request() {
		std::cout << "Bad request" << std::endl;
		_closed = true;
		return false;
	}

	bool	closed() {
		if (!_closed) {
			std::map<std::string, std::string>::const_iterator
				it = _headers.find("Connection");
			if (it != _headers.end()) {
				if (it->second == "close") {
					_closed = true;
					return true;
				}
			}
		}
		return _closed;
	}

	void	__repr__() {
		std::cout << "Request{method: " << Models::resolve_method(_method)
			<< ", uri: " << _uri
			<< ", http_version: " << _http_version << "}" << std::endl;

		std::map<std::string, std::string>::iterator it;
		for (it = _headers.begin(); it != _headers.end(); it++) {
			std::cout << "{" << it->first << ": " << it->second << "}" << std::endl;
		}
	}

	EMethod		get_method() const { return _method; }
	std::string get_uri() const { return _uri; }
	std::string get_header_value(const std::string &headerName) const {
		std::map<std::string, std::string>::const_iterator it;
		it = _headers.find(headerName);
		if (it == _headers.end()) {
			return "";
		}
		else {
			return it->second;
		}
	}
	bool		get_headers_status() { return _headers_done; }
	const struct timeval *get_time() const { return &_time; }

	void		set_headers_status(bool status) { _headers_done = status; }

 private:
	void	_parse(std::string buffer) {
		if (!_extract_method(&buffer))
			return;
		if (!_extract_uri(&buffer))
			return;
		if (!_extract_http_version(&buffer))
			return;
	}

	bool	_extract_method(std::string *buffer) {
		size_t	method_separator_pos = buffer->find(" ");
		if (method_separator_pos == std::string::npos)
			return bad_request();

		std::string	method_str = buffer->substr(0, method_separator_pos);
		_method = Models::get_method(method_str);
		buffer->erase(0, method_separator_pos + 1);
		return true;
	}

	bool	_extract_uri(std::string *buffer) {
		size_t	uri_separator_pos = buffer->find(" ");
		if (uri_separator_pos == std::string::npos)
			return bad_request();

		_uri = buffer->substr(0, uri_separator_pos);
		buffer->erase(0, uri_separator_pos + 1);
		return true;
	}

	bool	_extract_http_version(std::string *buffer) {
		size_t	version_separator_pos = buffer->find("\r\n");
		if (version_separator_pos == std::string::npos)
			return bad_request();

		_http_version = buffer->substr(0, version_separator_pos);
		buffer->erase(0, version_separator_pos + 2);
		return true;
	}

	void	_extract_headers(std::string *buffer) {
		size_t	header_separator_pos = buffer->find("\r\n");
		while (header_separator_pos != std::string::npos) {
			std::string	header_str = buffer->substr(0, header_separator_pos);
			size_t		header_name_separator_pos = header_str.find(":");
			if (header_name_separator_pos == std::string::npos)
				break;

			std::string	header_name = header_str.substr(0, header_name_separator_pos);
			std::string	header_value = header_str.substr(header_name_separator_pos + 2);
			_headers[header_name] = header_value;
			buffer->erase(0, header_separator_pos + 2);
			header_separator_pos = buffer->find("\r\n");
		}
	}

	void	_extract_body(__attribute__((unused)) std::string *buffer) {

	}
};
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_REQUEST_HPP_
