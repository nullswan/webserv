#ifndef SERVER_CGI_HPP_
#define SERVER_CGI_HPP_

#include <map>

namespace Webserv {
namespace Server {

class CGI {
 private:
 	std::map<std::string, std::string> _headers;

	int	fds[2];
	int	pid;

 public:
	CGI();
	~CGI();

	void	build_headers() {

	};
};

}  // namespace Server
}  // namespace Webserv

#endif  // SERVER_CGI_HPP_
