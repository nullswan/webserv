#ifndef MODELS_ENUMS_HPP_
#define MODELS_ENUMS_HPP_

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

}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ENUMS_HPP_
