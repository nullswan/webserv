#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <map>
#include <vector>
#include <string>

#include "codes.hpp"
#include "request.hpp"
#include "../models/IServer.hpp"
#include "../server/autoindex.hpp"

namespace Webserv {
namespace HTTP {
class Response {
	typedef Webserv::Models::IServer 				IServer;
	typedef std::map<std::string, std::string>		HeadersObject;

 private:
	std::string _body;
	std::string _payload;

	HeadersObject _headers;

	int _status;

	Request *_req;
	IServer	*_master;

 public:
	explicit Response(Request *request)
	:	_status(request->get_code()), _req(request) {}

	explicit Response(int code)
	:	_status(code), _req(0) {}

	bool	prepare(IServer *master) {
		_master = master;
		if (_req && _status < 400) { invoke(); }
		if (_status >= 400) {
			if (_req) {
				_body = master->get_error_page(_status, _req->get_host(), _req->get_uri());
				if (_body == "")
					_body = generate_status_page(_status);
			} else {
				_body = generate_status_page(_status);
			}
		}
		_payload = _prepare_headers() + _body + "\r\n";
		return true;
	}

	int		status() const { return _status; }
	void	set_status(int status) { _status = status; }
	const void *toString() const { return _payload.c_str(); }
	size_t	size() const { return _payload.size(); }

 private:
	void	GET(const Models::ILocation *loc) {
		_get_file_location(loc);
	}

	bool	_get_cgi(const Models::ILocation *loc) {
		std::string cgi_path;
		if (loc)
			cgi_path = loc->get_cgi(_req->get_uri());
		else
			cgi_path = _master->get_cgi(_req->get_uri());
		if (cgi_path != "")
			std::cout << "[deprecated] appears to be a cgi path" << std::endl;
		return false;
	}

	bool _dump_files_dir(const std::string path,
		std::vector<struct dirent> *bucket) {
		errno = 0;

		DIR	*dirptr = opendir(path.c_str());
		if (!dirptr) {
			_status = 500;
			if (errno == EACCES)
				_status = 403;
			if (errno == ENOENT)
				_status = 404;
			return false;
		}

		struct dirent *file;
		while ((file = readdir(dirptr))) {
			if (errno) {
				_status = 500;
				closedir(dirptr);
				return false;
			}
			bucket->push_back(*file);
		}
		closedir(dirptr);
		return true;
	}

	bool	_get_index(const Models::ILocation *loc,
		const std::string &path,
		const std::vector<struct dirent> &files) {
		Models::IBlock::IndexObject indexs;
		if (loc)
			indexs = loc->get_indexs();
		else
			indexs = _master->get_indexs();
		if (indexs.size() <= 0)
			return false;

		Models::IBlock::IndexObject::const_iterator it;
		for (it = indexs.begin(); it != indexs.end(); it++) {
			std::vector<struct dirent>::const_iterator	it_file;
			for (it_file = files.begin(); it_file != files.end(); it_file++)
				if (it_file->d_name == *it)
					return _get_file_path(loc, path + "/" + it_file->d_name);
		}
		return false;
	}

	bool	_get_autoindex(const std::vector<struct dirent>& files,
		const std::string &path, const std::string &root) {
		Server::AutoIndexBuilder autoindex(files, root, path);
		_body = autoindex.toString();
		_status = 200;
		return true;
	}

	bool	_get_file_location(const Models::ILocation *loc) {
		std::string path;
		if (!loc) {
			if (_master->get_redirection() != "")
				return _get_redirection(_master->get_redirection(),
					_master->get_redirection_code());
			path = _master->get_root();
		} else {
			if (loc->get_redirection() != "")
				return _get_redirection(loc->get_redirection(),
					loc->get_redirection_code());
			path = loc->get_root();
		}
		path += _req->get_uri();
		return _get_file_path(loc, path);
	}

	bool	_get_redirection(const std::string &path, const int &code) {
		_status = code;
		_headers["Location"] = path;
		return true;
	}

	bool	_get_file_path(const Models::ILocation *loc, const std::string &path) {
		_get_cgi(loc);

		if (path[path.size() - 1] == '/')
			return _get_dir(loc, path);

		errno = 0;
		struct stat db;
		if (stat(path.c_str(), &db) == -1) {
			_status = 500;
			if (errno == ENOENT || errno == ENOTDIR)
				_status = 404;
			return true;
		}

		switch (db.st_mode & S_IFMT) {
			case S_IFDIR:
				return _get_dir(loc, _req->get_uri());
			default: {
				_body = _get_file_content(path);
				return true;
			}
		}
		_status = 404;
		return true;
	}

	bool	_get_dir(const Models::ILocation *loc, const std::string path) {
		std::vector<struct dirent> files;
		if (!_dump_files_dir(path, &files))
			return false;
		if (_get_index(loc, path, files))
			return true;
		if (loc && loc->get_autoindex())
			return _get_autoindex(files, _req->get_uri(), loc->get_root());
		else if (!loc && _master->get_autoindex())
			return _get_autoindex(files, _req->get_uri(), _master->get_root());
		_status = 404;
		return false;
	}

	void	invoke() {
		const Models::ILocation	*loc = _master->get_location_using_vhosts(
			_req->get_host(), _req->get_uri());

		if (_req->get_method() == METH_GET)
			GET(loc);
		// else if (_req->get_method() == METH_POST)
		// else if (_req->get_method() == METH_DELETE)
		else
			_status = 501;
	}

	std::string _prepare_headers() {
		_headers["Connection"] = "keep-alive";
		_headers["Content-Type"] = "text/html; charset=utf-8";
		_headers["Content-Length"] = _toString(_body.size());
		_set_header_date();
		_headers["Server"] = WEBSERV_SERVER_VERSION;
		#ifdef WEBSERV_BUILD_COMMIT
			_headers["Server"] += WEBSERV_BUILD_COMMIT;
		#endif

		std::stringstream head;
		head << "HTTP/1.1 " << _status << " " << resolve_code(_status) << "\r\n";

		std::string headers;
		HeadersObject::iterator it = _headers.begin();
		for (; it != _headers.end(); ++it)
			headers += it->first + ": " + it->second + "\r\n";

		return head.str() + headers + "\r\n";
	}

	void	_set_header_date() {
		std::time_t t = std::time(NULL);
		char buf[30];

		strftime(buf, sizeof(buf), "%a, %d %b %Y %T %Z", std::localtime(&t));
		_headers["Date"] = std::string(buf);
	}

	static std::string _toString(size_t size) {
		std::stringstream ss;
		ss << size;
		return ss.str();
	}

	std::string _get_file_content(const std::string &path) {
		int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd == -1) {
			_status = 500;
			if (errno == EACCES)
				_status = 403;
			return "";
		}

		ssize_t n;
		char buf[1024];
		std::stringstream ss;
		while ((n = read(fd, buf, sizeof(buf))) > 0)
			ss.write(buf, n);
		close(fd);
		return ss.str();
	}
};
}  // namespace HTTP
}  // namespace Webserv

#endif  // HTTP_RESPONSE_HPP_
