/*
	http://nginx.org/en/docs/http/ngx_http_core_module.html#location
	
	Interface that represent a location block inside a server block in the configuration file.
*/

#ifndef MODELS_ILOCATION_HPP_
#define MODELS_ILOCATION_HPP_

#include "IBlock.hpp"

namespace Webserv {
namespace Models {
class ILocation : public Webserv::Models::IBlock {
 public:
	ILocation() {}

	ILocation(const ILocation &ref)
	: IBlock(ref) {}

	ILocation *clone() {
		return new ILocation(*this);
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ILOCATION_HPP_
