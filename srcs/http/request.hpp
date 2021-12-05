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
	typedef Webserv::Models::ERead								ERead;
	typedef Webserv::Models::ECode								ECode;

 private:
	struct timeval _time;

	std::string	_raw_request;

	EMethod		_method;
	std::string _host;
	std::string	_uri;
	std::string _http_version;

	std::map<std::string, std::string>	_headers;
	std::map<std::string, std::string>	_form;

	EPostForm	_post_form;
	size_t		_body_size;
	std::string	_multipart_boundary;

	bool	_headers_ready;
	bool	_body_ready;
	bool	_chunked;
	bool	_closed;
	ECode	_http_code;

 public:
	explicit Request(std::string buffer) : _raw_request(buffer),
		_method(Models::METHOD_UNKNOWN), _uri(""), _http_version(""),
		_headers(), _form(),
		_post_form(Models::POST_FORM_UNKNOWN), _body_size(0), _multipart_boundary(""),
		_headers_ready(false), _body_ready(false),
		_chunked(false), _closed(false) {
		gettimeofday(&_time, NULL);
	}

	void	handle_buffer(std::string buffer) { _raw_request += buffer; }

	bool	init() {
		if (_extract_method() == false)
			return false;
		if (_extract_uri() == false)
			return false;
		if (_extract_http_version() == false)
			return false;
		_extract_headers(&_headers);
		if (_validate_host() == false)
			return false;
		if (_method == Models::POST && _validate_post() == false)
			return false;
		_http_code = Models::NOT_FOUND;
		_raw_request.erase(0, 2);
		_headers_ready = true;
		return true;
	}

	bool	read_body() {
		if (_chunked == true) {
			if (_read_chunks() == Models::READ_WAIT) {
				return false;
			}
			_body_size = _raw_request.size();
			_chunked = false;
		}
		if (_raw_request.size() < _body_size) {
			return false;
		}
		if (_post_form == Models::URLENCODED) {
			_extract_urlencoded();
		} else if (_post_form == Models::MULTIPART) {
			_extract_multipart();
		}
		_body_ready = true;
		__repr__();
		return true;
	}

	bool	closed() {
		if (!_closed) {
			const_iterator it = _headers.find("connection");
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
	int	get_code() const { return _http_code; }
	const std::string get_raw_request() const { return _raw_request; }
	EMethod		get_method() const { return _method; }
	const std::string get_uri() const { return _uri; }
	const std::string get_host() const { return _host; }
	const std::string get_header_value(const std::string &headerName) const {
		const_iterator it = _headers.find(headerName);
		if (it == _headers.end()) {
			return "";
		} else {
			return it->second;
		}
	}
	bool		get_header_status() const { return _headers_ready; }


	void	__repr__() {
		std::cout << "Request{" << std::endl;
		std::cout << "\tmethod: " << Models::resolve_method(_method)
			<< ", " << std::endl;
		std::cout << "\turi: " << _uri << ", " << std::endl;
		std::cout << "\thttp_version: " << _http_version << ", " << std::endl;
		std::cout << "\theaders: {" << std::endl;

		std::map<std::string, std::string>::iterator it;
		for (it = _headers.begin(); it != _headers.end(); it++) {
			std::cout << "\t\t" << it->first << ": " << it->second << ", " << std::endl;
		}

		if (_method == Models::POST) {
			std::cout << "\t}," << std::endl;
			std::cout << "\tform: {" << std::endl;
			std::map<std::string, std::string>::iterator it2;
			for (it2 = _form.begin(); it2 != _form.end(); it2++) {
				std::cout << "\t\t" << it2->first << ": "
					<< it2->second << ", " << std::endl;
			}
			std::cout << "\t}" << std::endl;
			std::cout << "\tbody: {" << std::endl;
			std::cout << _raw_request << std::endl;
		}
		std::cout << "\t}" << std::endl;
		std::cout << "}" << std::endl;
	}

 private:
	ERead	_read_chunks() {
		if (_raw_request.find("0\r\n\r\n") == std::string::npos) {
			return Models::READ_WAIT;
		} else {
			std::string payload;
			do {
				const size_t header_end = _raw_request.find("\r\n");
				if (header_end == std::string::npos) {
					return Models::READ_ERROR;
				}
				const int64_t chunk_size = strtol(
						_raw_request.substr(0, header_end).c_str(), NULL, 16);
				_raw_request.erase(0, header_end + 2);
				if (chunk_size == 0) {
					_raw_request = payload;
					return Models::READ_OK;
				}
				payload += _raw_request.substr(0, chunk_size);
				_raw_request.erase(0, chunk_size + 2);
			} while (true);
		}
	}

	bool	_extract_method() {
		const size_t method_separator_pos = _raw_request.find(" ");
		if (method_separator_pos == std::string::npos)
			return _invalid_request(Models::BAD_REQUEST);
		const std::string method_str = _raw_request.substr(0, method_separator_pos);
		_method = Models::get_method(method_str);
		_raw_request.erase(0, method_separator_pos + 1);
		if (_method == Models::METHOD_UNKNOWN)
			return _invalid_request(Models::BAD_REQUEST);
		if (_method == Models::GET
			|| _method == Models::POST
			|| _method == Models::DELETE)
			return true;
		return _invalid_request(Models::NOT_IMPLEMENTED);
	}

	bool	_extract_uri() {
		const size_t uri_separator_pos = _raw_request.find(" ");
		if (uri_separator_pos == std::string::npos)
			return _invalid_request(Models::BAD_REQUEST);
		_uri = _raw_request.substr(0, uri_separator_pos);
		_strtolower(&_uri);
		if (_uri == "" || _uri.find("/") != 0)
			return _invalid_request(Models::BAD_REQUEST);
		_raw_request.erase(0, uri_separator_pos + 1);
		return true;
	}

	bool	_extract_http_version() {
		const size_t version_separator_pos = _raw_request.find("\r\n");
		if (version_separator_pos == std::string::npos)
			return _invalid_request(Models::BAD_REQUEST);
		_http_version = _raw_request.substr(0, version_separator_pos);
		_strtolower(&_http_version);
		size_t version_start;
		if (_http_version == "" || _http_version.find("http") != 0
			|| (version_start = _http_version.find("/")) == std::string::npos)
			return _invalid_request(Models::BAD_REQUEST);
		_http_version = _http_version.substr(version_start + 1);
		if (_http_version != "1.1")
			return _invalid_request(Models::HTTP_VERSION_NOT_SUPPORTED);
		_raw_request.erase(0, version_separator_pos + 2);
		return true;
	}

	void	_extract_headers(std::map<std::string, std::string> *bucket) {
		size_t	header_separator_pos = _raw_request.find("\r\n");
		while (header_separator_pos != std::string::npos) {
			std::string	header_str = _raw_request.substr(0, header_separator_pos);
			size_t		header_name_separator_pos = header_str.find(":");
			if (header_name_separator_pos == std::string::npos)
				break;
			std::string	header_name = header_str.substr(0, header_name_separator_pos);
			std::string	header_value = header_str.substr(header_name_separator_pos + 1);
			_strtolower(&header_name);
			_strtolower(&header_value);
			_trim(&header_value);
			(*bucket)[header_name] = header_value;
			if (header_name == "host")
				_host = header_value;
			_raw_request.erase(0, header_separator_pos + 2);
			header_separator_pos = _raw_request.find("\r\n");
		}
	}

	void	_extract_urlencoded() {
		size_t form_separator_pos;
		do {
			form_separator_pos = _raw_request.find("&");
			const std::string form_field = _raw_request.substr(0, form_separator_pos);
			const size_t form_field_separator_pos = form_field.find("=");
			if (form_field_separator_pos == std::string::npos) {
				break;
			}
			const std::string form_name = form_field.substr(0, form_field_separator_pos);
			const std::string form_value = form_field.substr(
					form_field_separator_pos + 1);
			_form[form_name] = form_value;
			_raw_request.erase(0, form_separator_pos + 1);
		} while (form_separator_pos != std::string::npos);
	}

	void	_extract_multipart() {
		size_t boundary = _raw_request.find(_multipart_boundary + "\r\n");
		while (boundary != std::string::npos) {
			_raw_request.erase(0, _multipart_boundary.size() + 2);
			std::map<std::string, std::string> boundary_headers;
			_extract_headers(&boundary_headers);
			_raw_request.erase(0, 2);
			const_iterator it = boundary_headers.find("content-disposition");
			if (it == boundary_headers.end() || it->second == ""
				|| it->second.find("form-data") == std::string::npos) {
				_invalid_request(Models::BAD_REQUEST);
				return;
			}
			const size_t form_name_start = it->second.find("name");
			if (form_name_start == std::string::npos) {
				_invalid_request(Models::BAD_REQUEST);
				return;
			}
			std::string form_name = it->second.substr(form_name_start + 5);
			size_t form_name_end = form_name.find(";");
			if (form_name_end != std::string::npos) {
				form_name.erase(form_name_end);
			}
			_trim(&form_name, "\"");
			const size_t form_value_end = _raw_request.find(_multipart_boundary);
			if (form_value_end == std::string::npos) {
				_invalid_request(Models::BAD_REQUEST);
				return;
			}
			std::string form_value = _raw_request.substr(0, form_value_end);
			_rtrim(&form_value, "\r\n");
			_form[form_name] = form_value;
			_raw_request.erase(0, form_value.size() + 2);
			boundary = _raw_request.find(_multipart_boundary + "\r\n");
		}
	}

	bool	_validate_host() {
		const_iterator it = _headers.find("host");
		if (it == _headers.end() || it->second == "") {
			return _invalid_request(Models::BAD_REQUEST);
		}
		return true;
	}

	bool	_validate_post() {
		const_iterator it = _headers.find("transfer-encoding");
		if (it != _headers.end() &&
			it->second.find("chunked") != std::string::npos) {
			_chunked = true;
		} else {
			it = _headers.find("content-length");
			if (it == _headers.end() || it->second == "") {
				return _invalid_request(Models::BAD_REQUEST);
			}
			_body_size = static_cast<size_t>(strtol(it->second.c_str(), NULL, 10));
			if (_body_size > 10000000)
				return _invalid_request(Models::PAYLOAD_TOO_LARGE);
		}
		it = _headers.find("content-type");
		if (it == _headers.end() || it->second == "") {
			return _invalid_request(Models::BAD_REQUEST);
		}
		if (it->second.find("application/x-www-form-urlencoded") == 0) {
			_post_form = Models::URLENCODED;
		} else if (it->second.find("multipart/form-data") == 0) {
			_post_form = Models::MULTIPART;
			size_t boundary_start = it->second.find("boundary");
			if (boundary_start == std::string::npos) {
				return _invalid_request(Models::BAD_REQUEST);
			}
			boundary_start += 9;
			const size_t boundary_size = it->second.substr(boundary_start).find(" ");
			_multipart_boundary = "--" + \
				it->second.substr(boundary_start, boundary_size);
		}
		return true;
	}

	bool	_invalid_request(ECode http_code) {
		// if (http_code == Models::BAD_REQUEST)
		// 	std::cout << "Bad request" << std::endl;
		// else if (http_code == Models::PAYLOAD_TOO_LARGE)
		// 	std::cout << "Payload too large" << std::endl;
		// else if (http_code == Models::NOT_IMPLEMENTED)
		// 	std::cout << "Http method not implemented" << std::endl;
		// else if (http_code == Models::HTTP_VERSION_NOT_SUPPORTED)
		// 	std::cout << "Http version not supported" << std::endl;
		_http_code = http_code;
		_closed = true;
		return false;
	}

	inline std::string* _strtolower(std::string *s) {
		for (std::string::iterator it = s->begin(); it != s->end(); it++)
			*it = std::tolower(*it);
		return s;
	}

	inline std::string* _rtrim(std::string *s, const char *t = " \t") {
	    s->erase(s->find_last_not_of(t) + 1);
	    return s;
	}

	inline std::string* _ltrim(std::string *s, const char *t = " \t") {
	    s->erase(0, s->find_first_not_of(t));
	    return s;
	}

	inline std::string* _trim(std::string *s, const char *t = " \t") {
	    return _ltrim(_rtrim(s, t), t);
	}
};
}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_REQUEST_HPP_
