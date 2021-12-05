#ifndef HTTP_STATUS_HPP_
#define HTTP_STATUS_HPP_

#include <map>
#include <string>

namespace Webserv {
namespace Http {

static std::map<int, std::string> http_code_list = {
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{422, "Unprocessable Entity"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"},
};

const std::string	&resolve_code(const int &status_code) {
	std::map<int, std::string>::iterator it = http_code_list.find(status_code);
	if (it == http_code_list.end())
		return "Unknown error";
	return it->second;
}

const std::string	&generate_status_page(const int &status_code) {
	return "<!DOCTYPE html>"
		"<html>"
		"	<head>"
		"		<style>"
		"			html { color-scheme: light dark; }"
		"			body { width: 35em; margin: 0 auto;"
		"			font-family: Tahoma, Verdana, Arial, sans-serif; }"
		"		</style>"
		"		<title>" + status_code + " - " + resolve_code(status_code) + "</title>"
		"	</head>"
		"	<body>"
		"		<h1>" + status_code + " - " + resolve_code(status_code) + "/h1>"
		"		<hr />"
		#ifdef BUILD_COMMIT
		"		<p><em>" + WEBSERV_SERVER_VERSION + "</em></p>"
		#else
		"		<p><em>Webserv</em></p>"
		#endif
		"	</body>"
		"</html>";
}

}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_STATUS_HPP_
