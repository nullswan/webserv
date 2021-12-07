#ifndef CONF_ENUMS_HPP_
#define CONF_ENUMS_HPP_

namespace Webserv {
namespace Conf {

enum	E_CONF_TOKEN : int {
	CONF_EMPTY_TOKEN = 0,
	CONF_NOT_FOUND_TOKEN,
	CONF_ERRORENOUS_TOKEN,
	CONF_SERVER_NAME,
	CONF_SERVER_LISTEN,
	CONF_SERVER_OPENING,
	CONF_SERVER_LOCATION,
	CONF_BLOCK_CGI,
	CONF_BLOCK_ROOT,
	CONF_BLOCK_INDEX,
	CONF_BLOCK_CLOSING,
	CONF_BLOCK_AUTOINDEX,
	CONF_BLOCK_REDIRECT,
	CONF_BLOCK_ERROR_PAGE,
	CONF_BLOCK_BODY_LIMIT,
	CONF_BLOCK_UPLOAD_PASS,
	CONF_BLOCK_ALLOWED_METHODS,
	CONF_TOKEN_MAX,
};

static const char DEFAULT_CONF_FILE[] = "server {\n"
"	server_name	webserv;\n"
"\n"
"	index		index.html index.php;\n"
"	root		tests/www/html/;\n"
"\n"
"	location /ping {\n"
"		index ping.html;\n"
"		allowed_methods GET;\n"
"	}\n"
"	location /files {\n"
"		autoindex	on;\n"
"		root tests/www/html/;\n"
"	}\n"
"\n"
"}";

}  // namespace Conf
}  // namespace Webserv

#endif  // CONF_ENUMS_HPP_
