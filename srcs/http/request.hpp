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
	typedef Webserv::Models::EPostForm							EPostForm;

 private:
	struct timeval _time;

	std::string	_raw_request;

	EMethod		_method;
	std::string	_uri;
	std::string _http_version;

	std::map<std::string, std::string>	_headers;
	std::map<std::string, std::string>	_body;

	EPostForm	_post_form;
	size_t		_body_size;
	std::string	_body_boundary;

	bool	_headers_ready;
	bool	_body_ready;
	bool	_chunked;
	bool	_closed;

 public:
	explicit Request(std::string buffer) : _raw_request(buffer),
		_method(Models::METHOD_UNKNOWN), _uri(""), _http_version(""),
		_headers(), _body(),
		_post_form(Models::POST_FORM_UNKNOWN), _body_size(0), _body_boundary(""),
		_headers_ready(false), _body_ready(false),
		_chunked(false), _closed(false) {
		gettimeofday(&_time, NULL);
	}

	void	handle_buffer(std::string buffer) { _raw_request += buffer; }

	void	init() {
		if (!_extract_method(&_raw_request))
			return;
		if (!_extract_uri(&_raw_request))
			return;
		if (!_extract_http_version(&_raw_request))
			return;
		_extract_headers(&_raw_request);
		const_iterator it = _headers.find("Host");
		if (it == _headers.end() || it->second == "") {
			_bad_request();
			return ;
		}
		it = _headers.find("Transfer-Encoding");
		if (it != _headers.end() && it->second.find("chunked") != std::string::npos) {
			_chunked = true;
		}
		if (_method == Models::POST) {
			it = _headers.find("Content-Type");
			if (it == _headers.end()) {
				_bad_request();
				return ;
			}
			if (it->second.find("application/x-www-form-urlencoded") == 0) {
				_post_form = Models::URLENCODED;
				if (_chunked == false) {
					it = _headers.find("Content-Length");
					if (it == _headers.end()) {
						_bad_request();
						return ;
					}
					_body_size = static_cast<size_t>(atoi(it->second.c_str()));
				}
			}
			else if (it->second.find("multipart/form-data") == 0) {
				_post_form = Models::MULTIPART;
				size_t boundary_start = it->second.find("boundary");
				if (boundary_start == std::string::npos) {
					_bad_request();
					return ;
				}
				boundary_start += 10;
				size_t boundary_size = it->second.substr(boundary_start).find("\"");
				if (boundary_size == std::string::npos) {
					_bad_request();
					return ;
				}
				_body_boundary = "--" + it->second.substr(boundary_start, boundary_size);
			}
		}
		_raw_request = _raw_request.substr(2);
	}

	void	read_body() {
		if (_chunked == true) {
			if (_raw_request.find("0\r\n\r\n") == std::string::npos) {
				return ;
			}
			// do decoding and clean buffer (_raw_request);
			_body_size = _raw_request.size();
			_chunked = false;
		}
		else {
			if (_post_form == Models::URLENCODED) {
				if (_raw_request.size() < _body_size) {
					return ;
				}
				_raw_request = _raw_request.substr(0, _body_size);
				_extract_urlencoded();
				_body_ready = true;
			}
			else if (_post_form == Models::MULTIPART) {
				const std::string end_boundary = _body_boundary + "--";
				if (_raw_request.find(end_boundary) == std::string::npos) {
					return ;
				}
				_raw_request = _raw_request.substr(0, _raw_request.size() - end_boundary.size());
				_extract_multipart();
				_body_ready = true;
			}
		}
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

	const struct timeval *get_time() const { return &_time; }
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
	bool		get_header_status() const { return _headers_ready; }
	bool		get_body_status() const { return _body_ready; }
	bool		get_chunked_status() const { return _chunked; }

	void		set_header_status(bool status) { _headers_ready = status; }

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

 private:
	bool	_extract_method(std::string *buffer) {
		size_t	method_separator_pos = buffer->find(" ");
		if (method_separator_pos == std::string::npos)
			return _bad_request();

		std::string	method_str = buffer->substr(0, method_separator_pos);
		_method = Models::get_method(method_str);
		buffer->erase(0, method_separator_pos + 1);
		return true;
	}

	bool	_extract_uri(std::string *buffer) {
		size_t	uri_separator_pos = buffer->find(" ");
		if (uri_separator_pos == std::string::npos)
			return _bad_request();

		_uri = buffer->substr(0, uri_separator_pos);
		buffer->erase(0, uri_separator_pos + 1);
		return true;
	}

	bool	_extract_http_version(std::string *buffer) {
		size_t	version_separator_pos = buffer->find("\r\n");
		if (version_separator_pos == std::string::npos)
			return _bad_request();

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
			std::string	header_content = header_str.substr(header_name_separator_pos + 1);
			std::string header_value = _trim(header_content);
			_headers[header_name] = header_value;
			buffer->erase(0, header_separator_pos + 2);
			header_separator_pos = buffer->find("\r\n");
		}
	}

	void	_extract_urlencoded() {
		size_t body_separator_pos;
		do {
			body_separator_pos = _raw_request.find("&");
			const std::string body_entry = _raw_request.substr(0, body_separator_pos);
			const size_t body_entry_separator_pos = body_entry.find("=");
			if (body_entry_separator_pos == std::string::npos) {
				break ;
			}
			const std::string body_field = body_entry.substr(0, body_entry_separator_pos);
			const std::string body_value = body_entry.substr(body_entry_separator_pos + 1);
			_body[body_field] = body_value;
			_raw_request.erase(0, body_separator_pos + 1);
		} while (body_separator_pos != std::string::npos);
	}

	void	_extract_multipart() {
		// size_t boundary = _raw_request.find(_body_boundary);
		// while (boundary != std::string::npos) {
		// 	std::string	header_str = _raw_request.substr(0, boundary);
		// 	size_t		header_name_separator_pos = header_str.find(":");
		// 	if (header_name_separator_pos == std::string::npos)
		// 		break;
		// 	std::string	header_name = header_str.substr(0, header_name_separator_pos);
		// 	std::string	header_content = header_str.substr(header_name_separator_pos + 1);
		// 	std::string header_value = _trim(header_content);
		// 	_headers[header_name] = header_value;
		// 	buffer->erase(0, header_separator_pos + 2);
		// 	header_separator_pos = buffer->find("\r\n");
		// }
	}

	bool	_bad_request() {
		std::cout << "Bad request" << std::endl;
		_closed = true;
		return false;
	}

	inline std::string& _rtrim(std::string& s, const char* t = " \t") {
	    s.erase(s.find_last_not_of(t) + 1);
	    return s;
	}

	inline std::string& _ltrim(std::string& s, const char* t = " \t") {
	    s.erase(0, s.find_first_not_of(t));
	    return s;
	}

	inline std::string& _trim(std::string& s, const char* t = " \t") {
	    return _ltrim(_rtrim(s, t), t);
	}
};
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_REQUEST_HPP_
