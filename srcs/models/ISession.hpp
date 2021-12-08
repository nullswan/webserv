#ifndef MODELS_ISESSION_HPP_
#define MODELS_ISESSION_HPP_

#include <map>
#include <string>

#include "consts.hpp"

class Session {
 public:
	std::map<std::string, std::string> cookies;

 private:
	std::string	_id;
	time_t		_expire;


 public:
	explicit Session(const std::string &id)
	:	_id(id),
		_expire(time(NULL) + WEBSERV_SESSION_TIMEOUT) {}

	bool	alive(time_t ttime) {
		return ttime > _expire;
	}

	// void	refresh(time_t time) {
	// 	_expire = time(NULL) + WEBSERV_SESSION_TIMEOUT;
	// }
};

#endif  // MODELS_ISESSION_HPP_

/*
	On Prepare Request:
		if Client has sid
			do Nothing
		if Client has no sid
			grep sid from cookies
			if found
				merge cookies
			it not found
				generate one

	On Send Request:
		grep each Set-Cookies:
			if Match PREFIX
				store in cookie_jar
*/
