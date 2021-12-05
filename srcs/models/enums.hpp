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

std::string	resolve_decorated_http_code(int status_code) {
	std::stringstream code;
	code << status_code;

	if (status_code > 600 ||  status_code < 100)
		return "?";
	if (status_code >= 500)
		return "\033[0;41;37m " + code.str() + " \033[0m";
	if (status_code >= 400)
		return "\033[0;43;37m " + code.str() + " \033[0m";
	if (status_code >= 300)
		return "\033[0;47;30m " + code.str() + " \033[0m";
	if (status_code >= 200)
		return "\033[0;42;37m " + code.str() + " \033[0m";
	if (status_code >= 100)
		return "\033[0;41;37m " + code.str() + " \033[0m";
	return " ? ";
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
			return "?";
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
