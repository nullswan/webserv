#ifndef MODELS_ENUMS_HPP_
#define MODELS_ENUMS_HPP_

#include <string>

namespace Webserv {
namespace Models {

enum EMethods {
	GET = 0,
	POST,
	DELETE,
	METHODS_TOTAL,
	METHOD_UNKNOWN
};

enum ERead {
	READ_OK = 0,
	READ_EOF,
	READ_ERROR,
	READ_WAIT
};

EMethods	get_method(const std::string& method) {
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
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
