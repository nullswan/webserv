#ifndef CONF_ERRORS_HPP_
#define CONF_ERRORS_HPP_

#include <string>
#include <iostream>

namespace Webserv {
namespace Conf {

bool
unexpected_token_line_error(const std::string &token, const int &line_nbr) {
	std::cerr << "unexpected token '" << token
		<< "' at line " << line_nbr << std::endl;
	return false;
}

bool	errorneous_line_error(const std::string &line, const int &line_nbr) {
	std::cerr << "unexpected sentence '" << line
		<< "' at line " << line_nbr << std::endl;
	return false;
}

bool	cannot_open_error(const std::string &file) {
	std::cerr << "cannot open file '" << file << "'" << std::endl;
	return false;
}

bool	insufficient_permission_error(const std::string &file) {
	std::cerr << "insufficient permission to open file '"
		<< file << "'" << std::endl;
	return false;
}

bool	file_not_found_error(const std::string &file) {
	std::cerr << "file '" << file << "' not found" << std::endl;
	return false;
}

bool	file_is_directory_error(const std::string &file) {
	std::cerr << "file '" << file << "' is a directory" << std::endl;
	return false;
}

bool	config_file_extension_error(const std::string &file) {
	std::cerr << "file '" << file
		<< "' has an invalid extension: must endswith .conf" << std::endl;
	return false;
}

bool	usage_error(const std::string &file) {
	std::cerr << "usage: " << file << " <config_file=optional>" << std::endl;
	return false;
}

bool	no_config_file_error() {
	std::cerr << "no config file specified" << std::endl;
	return false;
}

bool	use_default_configuration() {
	std::cout << "using default configuration file" << std::endl;
	return true;
}

bool	invalid_delimiter_error(const std::string &line) {
	std::cerr << "invalid delimiter: "
		<< line.substr(0, line.size() - 1)
		<< ">" << line.substr(line.size() - 1, line.size())
		 << "<" << std::endl;
	return false;
}

bool	unknown_method_error(const std::string &method) {
	std::cerr << "unknown or unsupported method: " << method << std::endl;
	return false;
}

bool	invalid_http_code_error(const int &code) {
	std::cerr << "Code " << code << " is invalid in HTTP/1.x" << std::endl;
	return false;
}

bool	invalid_value_error(const std::string &line, const int &line_nbr) {
	std::cerr << "invalid value '" << line << "' at line "
		<< line_nbr << std::endl;
	return false;
}

bool	invalid_scope_error(const std::string &line, const int &line_nbr) {
	std::cerr << "out of scope '" << line << "' at line "
		<< line_nbr << std::endl;
	return false;
}

bool	not_implemented_method_error(const std::string &method) {
	std::cerr << "method '" << method << "' is not implemented" << std::endl;
	return false;
}

}  // namespace Conf
}  // namespace Webserv

#endif  // CONF_ERRORS_HPP_
