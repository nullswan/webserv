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
	typedef std::map<std::string, std::string>::const_iterator	const_iterator;
	typedef Webserv::Models::EMethods							EMethod;

 private:
	struct timeval _time;

	std::string	_raw_request;

	EMethod		_method;
	std::string	_uri;
	std::string _http_version;

	std::map<std::string, std::string>	_headers;
	std::map<std::string, std::string>	_body;

	bool	_closed;

 public:
	explicit Request(std::string buffer) : _raw_request(buffer),
		_method(Models::METHOD_UNKNOWN), _uri(""), _http_version(""),
		_headers(), _closed(false) {
		gettimeofday(&_time, NULL);
	}

	void	init(std::string buffer) { _raw_request += buffer; }

	void	parse() {
		if (!_extract_method(&_raw_request))
			return;
		if (!_extract_uri(&_raw_request))
			return;
		if (!_extract_http_version(&_raw_request))
			return;
		_extract_headers(&_raw_request);
		if (_method == Models::POST)
			_extract_body(&_raw_request);
		__repr__();
	}

	bool	bad_request() {
		std::cout << "Bad request" << std::endl;
		_closed = true;
		return false;
	}

	bool	closed() {
		if (!_closed) {
			const_iterator it = _headers.find("Connection");
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

		std::map<std::string, std::string>::iterator it2;
		for (it2 = _body.begin(); it2 != _body.end(); it2++) {
			std::cout << "{" << it2->first << ": " << it2->second << "}" << std::endl;
		}
	}

	std::string get_raw_request() const { return _raw_request; };
	EMethod		get_method() const { return _method; }
	std::string get_uri() const { return _uri; }
	std::string get_header_value(const std::string &headerName) const {
		const_iterator it = _headers.find(headerName);
		if (it == _headers.end()) {
			return "";
		}
		else {
			return it->second;
		}
	}
	const struct timeval *get_time() const { return &_time; }

 private:
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

	void	_extract_body(std::string *buffer) {
		const_iterator form_type = _headers.find("Content-Type");
		if (form_type != _headers.end()) {
			if (form_type->second == "application/x-www-form-urlencoded") {
				*buffer = buffer->substr(2);
				const_iterator content_length = _headers.find("Content-Length");
				if (content_length == _headers.end()) {
					return ;	// Invalid x-www-form-urlencoded request: missing content length.
				}
				if (buffer->size() != static_cast<size_t>(atoi(content_length->second.c_str()))) {
					return ;	// Invalid x-www-form-urlencoded request: mismatching content length.
				}
				size_t body_separator_pos;
				do {
					body_separator_pos = buffer->find("&");
					const std::string body_entry = buffer->substr(0, body_separator_pos);
					const size_t body_entry_separator_pos = body_entry.find("=");
					if (body_entry_separator_pos == std::string::npos) {
						break ;
					}
					const std::string body_field = body_entry.substr(0, body_entry_separator_pos);
					const std::string body_value = body_entry.substr(body_entry_separator_pos + 1);
					_body[body_field] = body_value;
					buffer->erase(0, body_separator_pos + 1);
				} while (body_separator_pos != std::string::npos);
			}
			else if (form_type->second == "multipart/form-data") {
			}
		}
	}
};
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_REQUEST_HPP_
