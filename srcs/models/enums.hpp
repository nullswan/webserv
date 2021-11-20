#ifndef MODELS_ENUMS_HPP_
#define MODELS_ENUMS_HPP_

namespace Webserv {
namespace Models {

enum EMethods : int {
	GET = 0,
	POST,
	DELETE,
	TOTAL_METHODS
};

enum ERead : int {
	READ_OK = 0,
	READ_EOF,
	READ_ERROR
};

}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ENUMS_HPP_
