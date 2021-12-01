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
class IServer : public Webserv::Models::IBlock {
 public:
	typedef Webserv::Models::ILocation ILocation;

 protected:
	const std::string _name;
	const std::string _host;

	const int _port;

	std::vector<std::string> _indexs;
	std::map<std::string, ILocation *> _locations;
	std::map<int, std::string> _error_pages;

 public:
	IServer() : _name("_"), _host("0.0.0.0"), _port(8000) {
		/*
			in case we are super user 
			we set the port to 80, as nginx does
		*/
		if (getuid() == 0) {
			const_cast<int&>(_port) = 80;
		}
	}
	IServer(const std::string &name, const std::string &host, const int port)
		: _name(name), _host(host),  _port(port) {}
	IServer(const IServer &lhs)
		: _name(lhs._name), _host(lhs._host), _port(lhs._port),
		  _indexs(lhs._indexs), _locations(lhs._locations),
		  _error_pages(lhs._error_pages) {}
	~IServer() {}

	const std::string &get_name() const { return _name; }
	const std::string &get_host() const { return _host; }
	const std::string &get_ip() const { return _host; }
	int get_port() const { return _port; }

	const std::vector<std::string> &get_indexs() const { return _indexs; }
	const std::map<std::string, ILocation *> &get_locations() const {
		return _locations;
	}
	const std::map<int, std::string> &get_error_pages() const {
		return _error_pages;
	}

	void	add_index(const std::string &index) { _indexs.push_back(index); }
	void	add_location(const std::string &key, ILocation *location) {
		_locations.insert(std::pair<std::string, ILocation *>(key, location));
	}
	void	add_error_page(int code, const std::string &path) {
		_error_pages.insert(std::pair<int, std::string>(code, path));
	}
	void	set_name(const std::string &name) {
		const_cast<std::string&>(_name) = name;
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ISERVER_HPP_
