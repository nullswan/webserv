#ifndef HTTP_CODES_HPP_
#define HTTP_CODES_HPP_

#include <map>
#include <utility>
#include <string>
#include <sstream>

#include "http/enums.hpp"

namespace Webserv {
namespace HTTP {

static std::map<int, std::string> CODES;
static std::map<std::string, std::string> MIME_TYPES;

const std::string	resolve_code(const int &status_code) {
	std::map<int, std::string>::iterator it = CODES.find(status_code);
	if (it == CODES.end())
		return CODES[0];
	return it->second;
}

const std::string get_mime_type(const std::string &uri) {
	std::string ext;

	if (uri[uri.size() - 1] == '/')
		return MIME_TYPES[".html"];
	if (uri.find(".") != std::string::npos) {
		ext = uri.substr(uri.find("."));
	}
	if (ext != "") {
		std::map<std::string, std::string>::const_iterator it = MIME_TYPES.find(ext);
		if (it != MIME_TYPES.end())
			return it->second;
	}
	return MIME_TYPES[".bin"];
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

void	init_mime_types_map() {
	// reference: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
	MIME_TYPES["aac"] = "audio/aac";
	MIME_TYPES[".abw"] = "application/x-abiword";
	MIME_TYPES[".arc"] = "application/x-freearc";
	MIME_TYPES[".avi"] = "video/x-msvideo";
	MIME_TYPES[".azw"] = "application/vnd.amazon.ebook";
	MIME_TYPES[".bin"] = "application/octet-stream";
	MIME_TYPES[".bmp"] = "image/bmp";
	MIME_TYPES[".bz"] = "application/x-bzip";
	MIME_TYPES[".bz2"] = "application/x-bzip2";
	MIME_TYPES[".cda"] = "application/x-cdf";
	MIME_TYPES[".csh"] = "application/x-csh";
	MIME_TYPES[".css"] = "text/css";
	MIME_TYPES[".csv"] = "text/csv";
	MIME_TYPES[".doc"] = "application/msword";
	MIME_TYPES[".docx"] =
		"application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	MIME_TYPES[".eot"] = "application/vnd.ms-fontobject";
	MIME_TYPES[".epub"] = "application/epub+zip";
	MIME_TYPES[".gz"] = "application/gzip";
	MIME_TYPES[".gif"] = "image/gif";
	MIME_TYPES[".htm"] = "text/html";
	MIME_TYPES[".html"] = "text/html";
	MIME_TYPES[".ico"] = "image/vnd.microsoft.icon";
	MIME_TYPES[".ics"] = "text/calendar";
	MIME_TYPES[".jar"] = "application/java-archive";
	MIME_TYPES[".jpeg"] = "image/jpeg";
	MIME_TYPES[".jpg"] = "image/jpeg";
	MIME_TYPES[".js"] = "text/javascript";
	MIME_TYPES[".json"] = "application/json";
	MIME_TYPES[".jsonld"] = "application/ld+json";
	MIME_TYPES[".mid"] = "audio/midi";
	MIME_TYPES[".midi"] = "audio/x-midi";
	MIME_TYPES[".mjs"] = "text/javascript";
	MIME_TYPES[".mp3"] = "audio/mpeg";
	MIME_TYPES[".mp4"] = "video/mp4";
	MIME_TYPES[".mpeg"] = "video/mpeg";
	MIME_TYPES[".mpkg"] = "application/vnd.apple.installer+xml";
	MIME_TYPES[".odp"] = "application/vnd.oasis.opendocument.presentation";
	MIME_TYPES[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	MIME_TYPES[".odt"] = "application/vnd.oasis.opendocument.text";
	MIME_TYPES[".oga"] = "audio/ogg";
	MIME_TYPES[".ogv"] = "video/ogg";
	MIME_TYPES[".ogx"] = "application/ogg";
	MIME_TYPES[".opus"] = "audio/opus";
	MIME_TYPES[".otf"] = "font/otf";
	MIME_TYPES[".png"] = "image/png";
	MIME_TYPES[".pdf"] = "application/pdf";
	MIME_TYPES[".php"] = "application/x-httpd-php";
	MIME_TYPES[".ppt"] = "application/vnd.ms-powerpoint";
	MIME_TYPES[".pptx"] =
		"application/vnd.openxmlformats-officedocument.presentationml.presentation";
	MIME_TYPES[".rar"] = "application/x-rar-compressed";
	MIME_TYPES[".rtf"] = "application/rtf";
	MIME_TYPES[".sh"] = "application/x-sh";
	MIME_TYPES[".svg"] = "image/svg+xml";
	MIME_TYPES[".swf"] = "application/x-shockwave-flash";
	MIME_TYPES[".tar"] = "application/x-tar";
	MIME_TYPES[".tif"] = "image/tiff";
	MIME_TYPES[".tiff"] = "image/tiff";
	MIME_TYPES[".ttf"] = "font/ttf";
	MIME_TYPES[".ts"] = "video/mp2t";
	MIME_TYPES[".txt"] = "text/plain";
	MIME_TYPES[".vsd"] = "application/vnd.visio";
	MIME_TYPES[".wav"] = "audio/wav";
	MIME_TYPES[".weba"] = "audio/webm";
	MIME_TYPES[".webm"] = "video/webm";
	MIME_TYPES[".webp"] = "image/webp";
	MIME_TYPES[".woff"] = "font/woff";
	MIME_TYPES[".woff2"] = "font/woff2";
	MIME_TYPES[".xhtml"] = "application/xhtml+xml";
	MIME_TYPES[".xls"] = "application/vnd.ms-excel";
	MIME_TYPES[".xlsx"] =
		"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	MIME_TYPES[".xml"] = "application/xml";
	MIME_TYPES[".xul"] = "application/vnd.mozilla.xul+xml";
	MIME_TYPES[".zip"] = "application/zip";
	MIME_TYPES[".3gp"] = "video/3gpp";
	MIME_TYPES[".3g2"] = "video/3gpp2";
	MIME_TYPES[".7z"] = "application/x-7z-compressed";
}

void	init_status_map() {
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
	CODES[PAYLOAD_TOO_LARGE] = "Payload Too Large";
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
