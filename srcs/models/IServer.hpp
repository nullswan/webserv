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

	typedef std::map<std::string, ILocation *>	LocationObject;
	typedef std::map<std::string, IServer *>	VHostsObject;

 protected:
	const std::string _host;

	LocationObject	_locations;
	VHostsObject	_vhosts;

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

		LocationObject::const_iterator it;
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

		VHostsObject::const_iterator it5;
		for (it5 = lhs._vhosts.begin(); it5 != lhs._vhosts.end(); ++it5) {
			_vhosts[it5->first] = new IServer(*(it5->second));
		}
	}

	~IServer() {
		LocationObject::iterator loc_it = _locations.begin();
		for (; loc_it != _locations.end(); loc_it++)
			delete loc_it->second;

		VHostsObject::iterator vhost_it = _vhosts.begin();
		for (; vhost_it != _vhosts.end(); vhost_it++)
			delete vhost_it->second;
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

	// vHost(s)
	int vhosts_size() const { return _vhosts.size(); }
	const IServer *get_vhost(const std::string &host) const {
		if (_vhosts.size() > 0) {
			VHostsObject::const_iterator it = _vhosts.find(host);
			if (it != _vhosts.end())
				return it->second;
		}
		return this;
	}

	// Solver for error_pages behind vHosts or Location
	const std::string get_error_page(int status, const std::string &uri) const {
		LocationObject::const_iterator it = _locations.find(uri);
		if (it != _locations.end())
			return it->second->get_error_page(status);
		return IBlock::get_error_page(status);
	}

	const std::string
	get_error_page(int status, const std::string &host,
		const std::string &uri) const {
		const IServer *server = get_vhost(host);
		if (server)
			return server->get_error_page(status, uri);
		return get_error_page(status, uri);
	}

	// Other

	IServer *clone() const {
		return new IServer(*this);
	}

	void	merge(IServer *lhs) {
		std::string lowered_name = lhs->get_name();
		_vhosts.insert(std::pair<std::string, IServer *>(\
			*_strtolower(&lowered_name), lhs->clone()));
	}

 private:
	inline std::string* _strtolower(std::string *s) {
		for (std::string::iterator it = s->begin(); it != s->end(); it++)
			*it = std::tolower(*it);
		return s;
	}
};
}  // namespace Models
}  // namespace Webserv

#endif  // MODELS_ISERVER_HPP_
