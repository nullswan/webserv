/*
	Interface for shared methods accros cnf_block.
		-> Represented by a scope in configuration file
*/

#ifndef MODELS_IBLOCK_HPP_
#define MODELS_IBLOCK_HPP_

#include <map>
#include <vector>
#include <string>
#include <utility>

#include "enums.hpp"

namespace Webserv {
namespace Models {
class IBlock {
 public:
	typedef Webserv::Models::EMethods EMethods;

 protected:
	std::string _root;
	std::string _redirection;
	int			_redirection_code;

	size_t 		_body_limit;

	bool 		_methods_allowed[METHODS_TOTAL];
	bool 		_autoindex;

	std::vector<std::string> _indexs;
	std::map<int, std::string> _error_pages;
	std::map<std::string, std::string>	_cgi;

 public:
	IBlock()
	: 	_root(""),
		_redirection(""),
		_body_limit(1000000),
		_autoindex(false) {
		for (int i = 0; i < METHODS_TOTAL; i++) {
			_methods_allowed[i] = true;
		}
	}
	IBlock(const std::string &root, const std::string &redirection,
		const int &redirection_code, const size_t &body_limit,
		const bool method_allowed[METHODS_TOTAL],
		const bool &autoindex,
		const std::vector<std::string> &indexs,
		const std::map<int, std::string> error_pages)
	:	_root(root),
		_redirection(redirection),
		_redirection_code(redirection_code),
		_body_limit(body_limit),
		_autoindex(autoindex) {
		for (int i = 0; i < METHODS_TOTAL; i++)
			if (method_allowed[i])
				_methods_allowed[i] = true;

		std::vector<std::string>::const_iterator it = indexs.begin();
		for (; it != indexs.end(); ++it)
			_indexs.push_back(*it);

		std::map<int, std::string>::const_iterator it2 = error_pages.begin();
		for (; it2 != error_pages.end(); it++)
			_error_pages[it2->first] = it2->second;
	}

	~IBlock() {}

	// Root
	void set_root(const std::string& root) { _root = root; }
	std::string get_root() const { return _root; }

	// Redirection, redirection_code
	void set_redirection(const std::string& redirection, const int code) {
		_redirection = redirection;
		_redirection_code = code;
	}
	std::string get_redirection() const { return _redirection; }
	int get_redirection_code() const { return _redirection_code; }

	// Body Limit
	void set_body_limit(size_t limit) { _body_limit = limit; }
	size_t get_body_limit() const { return _body_limit; }

	// Allowed Methods
	void set_method(EMethods method, bool value) {
		_methods_allowed[method] = value;
	}
	bool get_method(EMethods method) const { return _methods_allowed[method]; }

	// Autoindex
	void set_autoindex(bool value) { _autoindex = value; }
	bool get_autoindex() const { return _autoindex; }

	// Index(s)
	const std::vector<std::string> &get_indexs() const { return _indexs; }
	void							add_index(const std::string &index) {
		_indexs.push_back(index);
	}

	// Error Pages
	const std::map<int, std::string> &get_error_pages() const {
		return _error_pages;
	}
	void	set_error_page(int code, const std::string &source) {
		_error_pages.insert(std::pair<int, std::string>(code, source));
	}

	// CGI
	void set_cgi(const std::string& extension, const std::string& cgi_path) {
		_cgi[extension] = cgi_path;
	}
	const std::map<std::string, std::string>& get_cgi() const {
		return _cgi;
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_IBLOCK_HPP_
