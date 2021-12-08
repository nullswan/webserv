#ifndef SERVER_AUTOINDEX_HPP_
#define SERVER_AUTOINDEX_HPP_

#include <dirent.h>

#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <algorithm>

namespace Webserv {
namespace Server {

class AutoIndexBuilder {
 private:
	typedef std::vector<std::pair<std::string, std::string> > IndexObject;

	std::stringstream _source;

	IndexObject _dirs;
	IndexObject _files;

 public:
	AutoIndexBuilder(const std::vector<struct dirent>& indexs,
		const std::string &path) {
		_build_header(path);
		_table_header();

		std::vector<struct dirent>::const_iterator it = indexs.begin();
		for (; it != indexs.end(); it++)
			_build_row(*it, path);

		_sort_indexs();
		_dump_indexs();
		_table_footer();
		_build_footer();
	}

	const std::string toString() const {
		return _source.str();
	}

 private:
	void	_build_row(struct dirent index, const std::string &path) {
		const std::string file_path = path + std::string(index.d_name);
		if (index.d_name[0] == '.' && strlen(index.d_name) == 1)
			return;
		struct stat st;
		std::cout << file_path << std::endl;
		if (stat(file_path.c_str(), &st) == -1)
			return;

		char	date[64];
		strftime(date, sizeof(date), "%d-%b-%Y %H:%M",
			localtime(&st.st_mtime));

		switch (st.st_mode & S_IFMT) {
			case S_IFDIR:
				return _build_row_dir(index.d_name, date);
			default:
				return _build_row_file(index.d_name, date, st.st_size);
		}
	}

	void	_build_row_file(const std::string &name, const std::string &mtime,
		const size_t size) {
		std::stringstream obj;

		obj << "<tr>\n"
			"<td><a href=\"" << name << "\">" << name << "</a></td>\n"
			"<td>" << mtime << "</td>\n"
			"<td>" << size << "</td>\n"
			"</tr>\n";
		_files.push_back(std::make_pair(name, obj.str()));
	}

	void	_build_row_dir(const std::string &name, const std::string &mtime) {
		std::stringstream obj;

		obj << "<tr>\n"
			"<td><a href=\"" << name << "/\">" << name << "</a></td>\n"
			"<td>" << mtime << "</td>\n"
			"<td>-</td>\n"
			"</tr>\n";
		_dirs.push_back(std::make_pair(name, obj.str()));
	}

	void	_build_header(const std::string &path) {
		_source << "<html>\n"
			"<head>\n"
			"<title>Webserv Autoindex</title>\n"
			"<style>\n"
			"html { color-scheme: light dark; }\n"
			"body { width: 35em; margin: 0 auto;\n"
			"font-family: monospace, Tahoma, Verdana, Arial, sans-serif;"
			"margin-top: 30px}\n"
			"</style>\n"
			"</head>\n"
			"<body>\n"
			"<h3>Index of " << path << "</h3>\n"
			"<hr />\n";
	}

	void	_build_footer() {
		_source << "<hr />\n"
			#ifdef WEBSERV_BUILD_COMMIT
				"<p><em>Autoindexed by "
				<< WEBSERV_SERVER_VERSION
				<< WEBSERV_BUILD_COMMIT << "</em></p>\n"
			#else
				"<p><em>Autoindexed by Webserv</h3></p>\n"
			#endif
			"</body>\n"
			"</html>";
	}

	void	_table_header() {
		_source << "<table style=\"width: 100%;text-align:left;;\">\n"
			"<thead>\n"
			"<tr>\n"
			"<th>Name</th>\n"
			"<th>Last Modified</th>\n"
			"<th>Size</th>\n"
			"</tr>\n"
			"</thead>\n"
			"<tbody>\n";
	}

	void	_table_footer() {
		_source << "\t\t\t</tbody>\n"
			"\t\t</table>\n";
	}

	void	_sort_indexs() {
		std::sort(_dirs.begin(), _dirs.end());
		std::sort(_files.begin(), _files.end());
	}

	void	_dump_indexs() {
		IndexObject::const_iterator it;
		for (it = _dirs.begin(); it != _dirs.end(); it++)
			_source << it->second;
		for (it = _files.begin(); it != _files.end(); it++)
			_source << it->second;
	}
};

}  // namespace Server
}  // namespace Webserv

#endif  // SERVER_AUTOINDEX_HPP_
