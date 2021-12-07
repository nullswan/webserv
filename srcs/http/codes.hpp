#ifndef HTTP_CODES_HPP_
#define HTTP_CODES_HPP_

#include <map>
#include <utility>
#include <string>
#include <sstream>

#include "enums.hpp"

namespace Webserv {
namespace HTTP {

static std::map<int, std::string> CODES;

const std::string	resolve_code(const int &status_code) {
	std::map<int, std::string>::iterator it = CODES.find(status_code);
	if (it == CODES.end())
		return CODES[0];
	return it->second;
}

const std::string	generate_status_page(const int &status_code) {
	std::stringstream	ss;
	ss << status_code;

	return "<!DOCTYPE html>\n"
		"<html>\n"
		"	<head>\n"
		"		<style>\n"
		"			html { color-scheme: light dark; }\n"
		"			body { width: 35em; margin: 0 auto;\n"
		"			font-family: monospace, Tahoma, Verdana, Arial, sans-serif; }\n"
		"		</style>\n"
		"		<title>" + ss.str() + " - " + resolve_code(status_code) + "</title>\n"
		"	</head>\n"
		"	<body>\n"
		"		<h1>" + ss.str() + " - " + resolve_code(status_code) + "</h1>\n"
		"		<hr />\n"
		#ifdef WEBSERV_BUILD_COMMIT
		"		<p><em>" + WEBSERV_SERVER_VERSION + WEBSERV_BUILD_COMMIT + "</em></p>\n"
		#else
		"		<p><em>Webserv</em></p>\n"
		#endif
		"	</body>\n"
		"</html>";
}

void init_status_map() {
	CODES[CONTINUE] = "Continue";
	CODES[SWITCHING_PROTOCOLS] = "Switching Protocols";
	CODES[PROCESSING] = "Processing";
	CODES[EARLY_HINTS] = "Early Hints";
	CODES[OK] = "OK";
	CODES[CREATED] = "Created";
	CODES[ACCEPTED] = "Accepted";
	CODES[NON_AUTHORITATIVE_INFORMATION] = "Non-Authoritative Information";
	CODES[NO_CONTENT] = "No Content";
	CODES[RESET_CONTENT] = "Reset Content";
	CODES[PARTIAL_CONTENT] = "Partial Content";
	CODES[MULTI_STATUS] = "Multi-Status";
	CODES[ALREADY_REPORTED] = "Already Reported";
	CODES[IM_USED] = "IM Used";
	CODES[MULTIPLE_CHOICES] = "Multiple Choices";
	CODES[MOVED_PERMANENTLY] = "Moved Permanently";
	CODES[FOUND] = "Found";
	CODES[SEE_OTHER] = "See Other";
	CODES[NOT_MODIFIED] = "Not Modified";
	CODES[USE_PROXY] = "Use Proxy";
	CODES[SWITCH_PROXY] = "Switch Proxy";
	CODES[TEMPORARY_REDIRECT] = "Temporary Redirect";
	CODES[PERMANENT_REDIRECT] = "Permanent Redirect";
	CODES[BAD_REQUEST] = "Bad Request";
	CODES[UNAUTHORIZED] = "Unauthorized";
	CODES[PAYMENT_REQUIRED] = "Payment Required";
	CODES[FORBIDDEN] = "Forbidden";
	CODES[NOT_FOUND] = "Not Found";
	CODES[METHOD_NOT_ALLOWED] = "Method Not Allowed";
	CODES[NOT_ACCEPTABLE] = "Not Acceptable";
	CODES[PROXY_AUTHENTICATION_REQUIRED] = "Proxy Authentication Required";
	CODES[REQUEST_TIMEOUT] = "Request Timeout";
	CODES[CONFLICT] = "Conflict";
	CODES[GONE] = "Gone";
	CODES[LENGTH_REQUIRED] = "Length Required";
	CODES[PRECONDITION_FAILED] = "Precondition Failed";
	CODES[REQUEST_ENTITY_TOO_LARGE] = "Request Entity Too Large";
	CODES[REQUEST_URI_TOO_LONG] = "Request URI Too Long";
	CODES[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
	CODES[REQUESTED_RANGE_NOT_SATISFIABLE] = "Requested Range Not Satisfiable";
	CODES[EXPECTATION_FAILED] = "Expectation Failed";
	CODES[IM_A_TEAPOT] = "I'm a teapot";
	CODES[UNPROCESSABLE_ENTITY] = "Unprocessable Entity";
	CODES[LOCKED] = "Locked";
	CODES[FAILED_DEPENDENCY] = "Failed Dependency";
	CODES[UPGRADE_REQUIRED] = "Upgrade Required";
	CODES[PRECONDITION_REQUIRED] = "Precondition Required";
	CODES[TOO_MANY_REQUESTS] = "Too Many Requests";
	CODES[REQUEST_HEADER_FIELDS_TOO_LARGE] = "Request Header Fields Too Large";
	CODES[UNAVAILABLE_FOR_LEGAL_REASONS] = "Unavailable For Legal Reasons";
	CODES[INTERNAL_SERVER_ERROR] = "Internal Server Error";
	CODES[NOT_IMPLEMENTED] = "Not Implemented";
	CODES[BAD_GATEWAY] = "Bad Gateway";
	CODES[SERVICE_UNAVAILABLE] = "Service Unavailable";
	CODES[GATEWAY_TIMEOUT] = "Gateway Timeout";
	CODES[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
	CODES[VARIANT_ALSO_NEGOTIATES] = "Variant Also Negotiates";
	CODES[INSUFFICIENT_STORAGE] = "Insufficient Storage";
	CODES[LOOP_DETECTED] = "Loop Detected";
	CODES[NOT_EXTENDED] = "Not Extended";
	CODES[NETWORK_AUTHENTICATION_REQUIRED] = "Network Authentication Required";
}

}  // namespace HTTP
}  // namespace Webserv

#endif  // HTTP_CODES_HPP_
