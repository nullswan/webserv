/*
	http://nginx.org/en/docs/http/ngx_http_core_module.html#location
	
	Interface that represent a location block inside a server block in the configuration file.
*/

#ifndef MODELS_ILOCATION_HPP_
#define MODELS_ILOCATION_HPP_

#include <map>
#include <string>

#include "IBlock.hpp"

namespace Webserv {
namespace Models {

class ILocation : public Webserv::Models::IBlock {
 private:
	const std::string	_path;

 public:
	explicit ILocation(std::string path) : _path(path) {}
	ILocation(const std::string &path,
		const std::string &root,
		const std::string &redirection,
		const int &redirection_code,
		const size_t &body_limit,
		const bool method_allowed[METHODS_TOTAL],
		const bool &autoindex,
		const std::vector<std::string> indexs,
		const std::map<int, std::string> error_pages)
	: IBlock(root, redirection, redirection_code,
		body_limit, method_allowed, autoindex, indexs, error_pages),
		_path(path) {}

	ILocation *clone() { return new ILocation(*this); }

	const std::string &get_path() const { return this->_path; }
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ILOCATION_HPP_
