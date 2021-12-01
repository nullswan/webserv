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
	int _redirection_code;

	size_t _body_limit;
	bool _methods_allowed[METHODS_TOTAL];
	bool _autoindex;

	std::map<std::string, std::string> _cgi;

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
	~IBlock() {}

	void set_method(EMethods method, bool value) {
		_methods_allowed[method] = value;
	}
	void set_body_limit(size_t limit) { _body_limit = limit; }
	void set_root(const std::string& root) { _root = root; }
	void set_redirection(const std::string& redirection, const int code) {
		_redirection = redirection;
		_redirection_code = code;
	}
	void set_cgi(const std::string& extension, const std::string& cgi_path) {
		_cgi[extension] = cgi_path;
	}

	bool get_method(EMethods method) const {
		return _methods_allowed[method];
	}
	size_t get_body_limit() const { return _body_limit; }
	std::string get_root() const { return _root; }
	std::string get_redirection() const { return _redirection; }
	const std::map<std::string, std::string>& get_cgi() const {
		return _cgi;
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_IBLOCK_HPP_
