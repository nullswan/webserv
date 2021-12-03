#ifndef MODELS_ENUMS_HPP_
#define MODELS_ENUMS_HPP_

#include <string>

namespace Webserv {
namespace Models {

enum EMethods {
	GET = 0,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
	PATCH,
	METHODS_TOTAL,
	METHOD_UNKNOWN
};

enum EPostForm {
	URLENCODED = 0,
	MULTIPART,
	POST_FORMS_TOTAL,
	POST_FORM_UNKNOWN
};

enum ERead {
	READ_OK = 0,
	READ_EOF,
	READ_ERROR,
	READ_WAIT
};

enum ECode {
	NOCODE = 0,
	BAD_REQUEST = 400,
	PAYLOAD_TOO_LARGE = 413,
	NOT_IMPLEMENTED = 501,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

EMethods	get_method(const std::string& method) {
	if (method == "GET")
		return GET;
	else if (method == "HEAD")
		return HEAD;
	else if (method == "POST")
		return POST;
	else if (method == "PUT")
		return PUT;
	else if (method == "DELETE")
		return DELETE;
	else if (method == "CONNECT")
		return CONNECT;
	else if (method == "OPTIONS")
		return OPTIONS;
	else if (method == "TRACE")
		return TRACE;
	else if (method == "PATCH")
		return PATCH;
	else
		return METHOD_UNKNOWN;
}

std::string	resolve_decorated_http_code(const std::string &status_code) {
	switch (status_code.c_str()[0]) {
		case '1':
			return "\033[0;41;37m " + status_code + " \033[0m";
		case '2':
			return "\033[0;42;37m " + status_code + " \033[0m";
		case '3':
			return "\033[0;47;37m " + status_code + " \033[0m";
		case '4':
			return "\033[0;43;37m " + status_code + " \033[0m";
		case '5':
			return "\033[0;41;37m " + status_code + " \033[0m";
		default:
			return " ? ";
	}
}

std::string	resolve_method(EMethods method) {
	switch (method) {
		case GET:
			return "GET";
		case POST:
			return "POST";
		case DELETE:
			return "DELETE";
		default:
			return "UNKNOWN";
	}
}

std::string resolve_decorated_method(EMethods method) {
	switch (method) {
		case GET:
			return "\033[0;42;37m GET    \033[0m";
		case POST:
			return "\033[0;46;37m POST   \033[0m";
		case DELETE:
			return "\033[0;41;37m DELETE \033[0m";
		default:
			return " UNKNOWN ";
	}
}

}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ENUMS_HPP_
