#ifndef HTTP_SESSION_HPP_
#define HTTP_SESSION_HPP_

#include <map>
#include <ctime>
#include <string>

#include "consts.hpp"

class Session {
	typedef std::map<std::string, std::string> Cookies;

 public:
	Cookies		cookies;

 private:
	std::string	_id;
	time_t		_expire;


 public:
	explicit Session(const std::string &id)
	:	_id(id),
		_expire(time(NULL) + WEBSERV_SESSION_TIMEOUT) {}

	bool	alive(time_t ttime) {
		return _expire > ttime;
	}

	bool	alive() {
		return _expire > time(NULL);
	}

	void	refresh() {
		_expire = time(NULL) + WEBSERV_SESSION_TIMEOUT;
	}
};

#endif  // HTTP_SESSION_HPP_
