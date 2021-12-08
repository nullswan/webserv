/*
	http://nginx.org/en/docs/http/ngx_http_core_module.html#location
	
	Interface that represent a location block inside a server block in the configuration file.
*/

#ifndef MODELS_ILOCATION_HPP_
#define MODELS_ILOCATION_HPP_

#include <map>
#include <string>
#include <vector>

#include "models/IBlock.hpp"

namespace Webserv {
namespace Models {

class ILocation : public Webserv::Models::IBlock {
 private:
	const std::string	_path;

 public:
	ILocation(const std::string &host, const int &port, const std::string &path)
	:	_path(path) {
		_name = host;
		_port = port;
	}
	ILocation(const ILocation &lhs)
	:	IBlock(lhs),
		_path(lhs._path) {}

	ILocation(const std::string &name,
		const int &port,
		const std::string &path,
		const std::string &root,
		const std::string &upload_pass,
		const size_t &body_limit,
		const std::map<int, std::string> error_pages)
	:	_path(path) {
		_name = name;
		_port = port;
		_root = root;
		_upload_pass = upload_pass;
		_body_limit = body_limit;
		_error_pages = error_pages;
	}

	ILocation *clone() { return new ILocation(*this); }

	const std::string &get_path() const { return this->_path; }
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ILOCATION_HPP_
