#ifndef HTTP_UTILS_HPP_
#define HTTP_UTILS_HPP_

#include <unistd.h>
#include <sys/time.h>

#include <string>
#include <sstream>

#include "http/enums.hpp"

namespace Webserv {
namespace HTTP {

static METHODS enumerate_method(const std::string &method) {
	if (method == "GET")
		return METH_GET;
	else if (method == "POST")
		return METH_POST;
	else if (method == "HEAD")
		return METH_HEAD;
	else if (method == "PUT")
		return METH_PUT;
	else if (method == "DELETE")
		return METH_DELETE;
	else if (method == "CONNECT")
		return METH_CONNECT;
	else if (method == "OPTIONS")
		return METH_OPTIONS;
	else if (method == "TRACE")
		return METH_TRACE;
	else if (method == "PATCH")
		return METH_PATCH;
	return METH_UNKNOWN;
}

static const std::string color_method(const METHODS &method) {
	switch (method) {
		case METH_GET:
			return "\033[0;42;37m GET     \033[0m";
		case METH_POST:
			return "\033[0;46;37m POST    \033[0m";
		case METH_DELETE:
			return "\033[0;41;37m DELETE  \033[0m";
		case METH_HEAD:
			return " HEAD    ";
		case METH_PUT:
			return " PUT     ";
		case METH_CONNECT:
			return " CONNECT ";
		case METH_OPTIONS:
			return " OPTIONS ";
		case METH_TRACE:
			return " TRACE   ";
		case METH_PATCH:
			return " PATCH   ";
		default:
			return " UNKNOWN ";
	}
}

static const std::string color_code(const STATUS_CODE &status_code) {
	std::stringstream code;
	code << status_code;

	if (status_code > NETWORK_AUTHENTICATION_REQUIRED
		||  status_code < CONTINUE)
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

std::string rand_string(const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
	std::string ret;
	ret.reserve(len);

	for (int i = 0; i < len; ++i)
		ret += alphanum[rand() % (sizeof(alphanum) - 1)];
	return ret;
}

}  // namespace HTTP
}  // namespace Webserv

#endif  // HTTP_UTILS_HPP_
