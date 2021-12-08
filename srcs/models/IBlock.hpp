/*
	Interface for shared methods accros cnf_block.
		-> Represented by a scope in configuration file
*/

#ifndef MODELS_IBLOCK_HPP_
#define MODELS_IBLOCK_HPP_

#include <unistd.h>
#include <limits.h>

#include <map>
#include <vector>
#include <string>
#include <utility>

#include "consts.hpp"

namespace Webserv {
namespace Models {
class IBlock {
 public:
	typedef std::map<int, std::string> 			ErrorPagesObject;
	typedef std::map<std::string, std::string>	CGIObject;
	typedef std::vector<std::string>			IndexObject;

 protected:
	std::string _name;
	int			_port;

	std::string _root;
	std::string _upload_pass;
	std::string _redirection;
	int			_redirection_code;

	size_t 		_body_limit;

	bool 		_methods_allowed[WEBSERV_METHODS_SUPPORTED];
	bool 		_autoindex;

	IndexObject 		_indexs;
	ErrorPagesObject	_error_pages;
	CGIObject			_cgi;

 public:
	IBlock()
	: 	_root(WEBSERV_DEFAULT_ROOT_DIR),
		_upload_pass(""),
		_redirection(""),
		_body_limit(1000000),
		_autoindex(false),
		_indexs(),
		_error_pages(),
		_cgi() {
		char cwd[PATH_MAX + 1];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
			_root = cwd;
		for (int i = 0; i < WEBSERV_METHODS_SUPPORTED; i++) {
			_methods_allowed[i] = true;
		}
	}

	~IBlock() {}

	// Name & Port
	const std::string	&get_name() const { return _name; }
	const int			&get_port() const { return _port; }

	// Root
	void set_root(const std::string& root) { _root = root; }
	const std::string &get_root() const { return _root; }

	// Upload Pass
	void set_upload_pass(const std::string& pass) { _upload_pass = pass; }
	const std::string &get_upload_pass() const { return _upload_pass; }

	// Redirection, redirection_code
	void set_redirection(const std::string& redirection, const int code) {
		_redirection = redirection;
		_redirection_code = code;
	}
	const std::string 	&get_redirection() const { return _redirection; }
	const int 			&get_redirection_code() const { return _redirection_code; }

	// Body Limit
	void set_body_limit(size_t limit) { _body_limit = limit; }
	const size_t &get_body_limit() const { return _body_limit; }

	// Allowed Methods
	void set_method(HTTP::METHODS method, bool value) {
		_methods_allowed[method] = value;
	}
	const bool &get_method(HTTP::METHODS method) const {
		return _methods_allowed[method];
	}
	void	deny_all_methods() {
		for (int i = 0; i < WEBSERV_METHODS_SUPPORTED; i++)
			_methods_allowed[i] = false;
	}

	// Autoindex
	void set_autoindex(bool value) { _autoindex = value; }
	const bool &get_autoindex() const { return _autoindex; }

	// Index(s)
	const IndexObject &get_indexs() const { return _indexs; }
	void							add_index(const std::string &index) {
		_indexs.push_back(index);
	}

	// Error Pages
	const ErrorPagesObject &get_error_pages() const {
		return _error_pages;
	}
	void	set_error_page(int code, const std::string &source) {
		_error_pages[code] = source;
	}
	const std::string get_error_page(int code) const {
		ErrorPagesObject::const_iterator it = _error_pages.find(code);
		if (it != _error_pages.end())
			return it->second;
		return "";
	}

	// CGI
	void			set_cgi(const std::string& extension, const std::string& cgi_path) {
		_cgi[extension] = cgi_path;
	}
	const CGIObject &get_cgis() const {
		return _cgi;
	}
	const std::string get_cgi(const std::string &uri) const {
		const std::string ext = uri.find(".") != std::string::npos
			? uri.substr(uri.find("."), uri.size()) : "";
		if (ext == "")
			return "";
		CGIObject::const_iterator it = _cgi.find(ext);
		if (it != _cgi.end())
			return it->second;
		return "";
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_IBLOCK_HPP_
