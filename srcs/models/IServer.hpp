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

 public:
	IServer() : _name("_"), _host("0.0.0.0"), _port(8000) {
		/*
			in case we are super user 
			we set the port to 80, as nginx does
		*/
		if (getuid() == 0) {
			set_port(80);
		}
	}

	IServer(const std::string &name, const std::string &host, const int port)
	: _name(name), _host(host),  _port(port) {}

	IServer(const IServer &lhs)
	: 	_name(lhs._name),
		_host(lhs._host),
		_port(lhs._port),
		_indexs(lhs._indexs) {
		std::map<std::string, ILocation *>::const_iterator it;
		for (it = lhs._locations.begin(); it != lhs._locations.end(); ++it) {
			_locations[it->first] = it->second->clone();
		}
	}

	~IServer() {
		std::map<std::string, ILocation *>::iterator it;
		for (it = _locations.begin(); it != _locations.end(); it++)
			delete it->second;
	}

	// Name
	const std::string &	get_name() const { return _name; }
	void				set_name(const std::string &name) {
		const_cast<std::string&>(_name) = name;
	}

	// Host:Port - IP
	const std::string &	get_host() const { return _host; }
	void				set_host(const std::string &host) {
		const_cast<std::string&>(_host) = host;
	}
	const std::string &	get_ip() const { return _host; }
	int 				get_port() const { return _port; }
	void				set_port(const int &port) {
		const_cast<int&>(_port) = port;
	}

	// Index(s)
	const std::vector<std::string> &get_indexs() const { return _indexs; }
	void							add_index(const std::string &index) {
		_indexs.push_back(index);
	}

	// Location(s)
	const std::map<std::string, ILocation *> &get_locations() const {
		return _locations;
	}
	ILocation	*new_location(const std::string &key) {
		ILocation *location = new ILocation(
			_root,
			_redirection, _redirection_code,
			_body_limit,
			_methods_allowed,
			_autoindex,
			_error_pages);

		_locations.insert(std::pair<std::string, ILocation *>(key, location));
		return location;
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ISERVER_HPP_
