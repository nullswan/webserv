#ifndef CONF_ENUMS_HPP_
#define CONF_ENUMS_HPP_

namespace Webserv {
namespace Conf {

enum	E_CONF_TOKEN : int {
	CONF_EMPTY_TOKEN = 0,
	CONF_ERRORENOUS_TOKEN,
	CONF_NOT_FOUND_TOKEN,
	CONF_SERVER_OPENING,
	CONF_SERVER_NAME,
	CONF_SERVER_LISTEN,
	CONF_SERVER_INDEX,
	CONF_SERVER_LOCATION,
	CONF_SERVER_ERROR_PAGE,
	CONF_BLOCK_CLOSING,
	CONF_BLOCK_ALLOWED_METHODS,
	CONF_BLOCK_CGI_PATH,
	CONF_BLOCK_BODY_LIMIT,
	CONF_BLOCK_REDIRECT,
	CONF_BLOCK_ROOT,
	CONF_BLOCK_AUTOINDEX,
	CONF_TOKEN_MAX,
};

static const std::string DEFAULT_CONF_FILE = "server {\n"
"	server_name	webserv;\n"
"\n"
"	index		index.html index.php;\n"
"\n"
"	location /ping {\n"
"		allowed_methods GET;\n"
"	}\n"
"\n"
"	location /google {\n"
"		redirect 301 http://google.fr/;\n"
"	}\n"
"}";

}  // namespace Conf
}  // namespace Webserv

#endif  // CONF_ENUMS_HPP_