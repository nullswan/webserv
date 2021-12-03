/*
	Interface representing a server block
*/

#ifndef MODELS_ISERVER_HPP_
#define MODELS_ISERVER_HPP_

#include <map>
#include <vector>
#include <string>
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
	const std::string _host;

	std::map<std::string, ILocation *> _locations;

 public:
	IServer() : _host("0.0.0.0") {
		/*
			in case we are super user 
			we set the port to 80, as nginx does
		*/
		_port = 8000;
		if (getuid() == 0)
			set_port(80);
	}

	IServer(const std::string &name, const std::string &host, const int port)
	:	_host(host) {
		_name = name;
		_port = port;
	}

	IServer(const IServer &lhs)
	:	_host(lhs._host) {
		_name = lhs._name;
		_port = lhs._port;

		std::map<std::string, ILocation *>::const_iterator it;
		for (it = lhs._locations.begin(); it != lhs._locations.end(); ++it) {
			_locations[it->first] = it->second->clone();
		}

		std::vector<std::string>::const_iterator it2;
		for (it2 = lhs._indexs.begin(); it2 != lhs._indexs.end(); ++it2) {
			_indexs.push_back(*it2);
		}

		std::map<int, std::string>::const_iterator it3;
		for (it3 = lhs._error_pages.begin(); it3 != lhs._error_pages.end(); ++it3) {
			_error_pages[it3->first] = it3->second;
		}

		std::map<std::string, std::string>::const_iterator it4;
		for (it4 = lhs._cgi.begin(); it4 != lhs._cgi.end(); ++it4) {
			_cgi[it4->first] = it4->second;
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

	// Location(s)
	const std::map<std::string, ILocation *> &get_locations() const {
		return _locations;
	}
	ILocation	*new_location(const std::string &key) {
		if (_locations.find(key) != _locations.end())
			return 0;
		ILocation *location = new ILocation(
			_name, _port,
			key,
			_root,
			_redirection, _redirection_code,
			_body_limit,
			_methods_allowed,
			_autoindex, _indexs,
			_error_pages);

		_locations.insert(std::pair<std::string, ILocation *>(key, location));
		return location;
	}

	void	merge(IServer *lhs) {
		std::map<std::string, ILocation *>::const_iterator it;
		for (it = lhs->_locations.begin(); it != lhs->_locations.end(); ++it) {
			_locations[it->first] = it->second->clone();
		}
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ISERVER_HPP_
