#ifndef HTTP_ERRORS_HPP_
#define HTTP_ERRORS_HPP_

#include <map>
#include <string>

#include "status.hpp"

namespace Webserv {
namespace Http {

const std::string	&generate_error_page(const int &status_code) {
	return "<!DOCTYPE html>"
	"<html>"
	"	<head>"
	"		<style>"
	"			html { color-scheme: light dark; }"
	"			body { width: 35em; margin: 0 auto;"
	"			font-family: Tahoma, Verdana, Arial, sans-serif; }"
	"		</style>"
	"		<title>" + status_code + " - "
		+ resolve_code(status_code) + "</title>"
	"	</head>"
	"	<body>"
	"		<h1>" + status_code + " - " resolve_code(status_code) + "/h1>"
	"		<p>Sorry, the page you are looking for does not exist.</p>"
	"		<hr />"
	"		<p><em>Webserv@" + BUILD_COMMIT + "</em></p>"
	"	</body>"
	"</html>";
}

}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_ERRORS_HPP_
