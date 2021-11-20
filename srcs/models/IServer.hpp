/*
	Interface representing a server block
*/

#ifndef MODELS_ISERVER_HPP_
#define MODELS_ISERVER_HPP_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "IBlock.hpp"
#include "ILocation.hpp"


namespace Webserv {
namespace Models {
class IServer : protected Webserv::Models::IBlock {
 public:
	typedef Webserv::Models::ILocation ILocation;

 protected:
	const std::string _name;  // ToDo:: Verify default value
	const std::string _host;  // ToDo: Verify default value

	const int _port;  // ToDo: Verify default value

	std::vector<std::string> _indexs;
	std::map<std::string, ILocation *> _locations;
	std::map<int, std::string> _error_pages;

 public:
	IServer(const std::string &name, const std::string &host, const int port)
		: _name(name), _host(host), _port(port) {}
	IServer(const IServer &lhs)
		: _name(lhs._name), _host(lhs._host), _port(lhs._port) {
		_indexs = lhs._indexs;
		_locations = lhs._locations;
		_error_pages = lhs._error_pages;
	}
	~IServer() {}

	const std::string &getName() const { return _name; }
	const std::string &getHost() const { return _host; }
	int getPort() const { return _port; }

	const std::vector<std::string> &getIndexs() const { return _indexs; }
	const std::map<std::string, ILocation *> &getLocations() const {
		return _locations;
	}
	const std::map<int, std::string> &getErrorPages() const {
		return _error_pages;
	}

	void	addIndex(const std::string &index) { _indexs.push_back(index); }
	void	addLocation(const std::string &key, ILocation *location) {
		_locations.insert(std::pair<std::string, ILocation *>(key, location));
	}
	void	addErrorPage(int code, const std::string &path) {
		_error_pages.insert(std::pair<int, std::string>(code, path));
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ISERVER_HPP_
