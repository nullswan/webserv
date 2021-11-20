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

	bool _have_redirection;
	std::string _redirection;
	int _redirection_code;

	size_t _body_limit;
	bool _methods_allowed[TOTAL_METHODS];
	bool _autoindex;  // ToDo: Verify default value

	std::map<std::string, std::string> _cgi;

 public:
	IBlock()
	: _root(""), _have_redirection(false), _redirection(""), _body_limit(1000000) {
		for (int i = 0; i < TOTAL_METHODS; i++) {
			_methods_allowed[i] = true;
		}
	}
	~IBlock() {}

	void setMethod(EMethods method, bool value) {
		_methods_allowed[method] = value;
	}
	void setBodyLimit(size_t limit) { _body_limit = limit; }
	void setRoot(const std::string& root) { _root = root; }
	void setRedirection(const std::string& redirection, const int code) {
		_have_redirection = true;
		_redirection = redirection;
		_redirection_code = code;
	}
	void setCgi(const std::string& extension, const std::string& cgi_path) {
		_cgi[extension] = cgi_path;
	}

	bool getMethod(EMethods method) const {
		return _methods_allowed[method];
	}
	bool haveRedirection() const { return _have_redirection; }
	size_t getBodyLimit() const { return _body_limit; }
	std::string getRoot() const { return _root; }
	std::string getRedirection() const { return _redirection; }
	const std::map<std::string, std::string>& getCgi() const {
		return _cgi;
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_IBLOCK_HPP_
