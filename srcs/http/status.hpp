#ifndef HTTP_STATUS_HPP_
#define HTTP_STATUS_HPP_

#include <map>
#include <string>

namespace Webserv {
namespace Http {

static std::map<int, std::string> status_code = {
	{403, "Forbidden"},
	{404, "Not found"},
	{500, "Server error"},
};

const std::string &resolve_code(const int &status_code) const {
	std::map<int, std::string>::iterator it = status_code.find(status_code);
	if (!it)
		return "Unknown error";
	return it->second;
}

}  // namespace Http
}  // namespace Webserv

#endif  // HTTP_STATUS_HPP_
