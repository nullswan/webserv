#ifndef MODELS_ENUMS_HPP_
#define MODELS_ENUMS_HPP_

#include <string>

namespace Webserv {
namespace Models {

enum EMethods : int {
	GET = 0,
	POST,
	DELETE,
	METHODS_TOTAL,
	METHOD_UNKNOWN
};

enum ERead : int {
	READ_OK = 0,
	READ_EOF,
	READ_ERROR
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
	switch (const status_code[0]) {
		case '1':
			return "Information";
		case '2':
			return "Success";
		case '3':
			return "Redirection";
		case '4':
			return "Client Error";
		case '5':
			return "Server Error";
		default:
			return "Unknown";
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
