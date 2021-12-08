#ifndef MODELS_ISESSION_HPP_
#define MODELS_ISESSION_HPP_

#include <map>
#include <string>

class Session {
 public:
	std::string id;
	std::map<std::string, std::string> cookies;
	time_t expire;

 public:
	bool	alive(time_t ttime) {
		return ttime > expire;
	}
};

#endif  // MODELS_ISESSION_HPP_
