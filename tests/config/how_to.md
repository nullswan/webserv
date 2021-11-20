# Server rules (IServer)
Define a server block
```
server (IServer) {

}
```

Server can contains specific name, host and port
- ToDo: Verify default values against nginx
```
server {
	server_name (IServer._name<std::string>)
	host (IServer._host<std::string>));
	port (IServer._port<int>);
}
```

Server can contains index, thus are the default page to be shown before autoindex.
```
server {
	index (IServer._index<std::vector<std::string>>);
}
```

Server can contains a list of locations, that act as sub-routes.
```
server {
	location key {}
	location key {}
	(IServer._locations<std::map<std::string key, ILocation *obj>>);
}
```

Server can contains different error pages to be rendered when a specific error occurs.
```
server {
	error_page (IServer._error_pages<std::map<int, std::string>>);
}
```

# Location Rules (ILocation)

Define a location block, thus are mapped in IServer object.
```
server {
	IServer._locations<std::string, ILocation *>
	location (ILocation) {

	}
}
```

# Shared Rules (IBlock)

Server or Location can use autoindex, thus will list all files in the directory. In either way, this would return a 403.
```
server {
	autoindex (IServer._autoindex<bool>);

	location /example/ {
		autoindex (ILocation._autoindex<bool>);
	}
}
```

Server and Location can serve file from a specific directory
```
server {
	root (IServer.IBlock._root<std::string>);

	location /example/ {
		root (ILocation.IBlock._root<std::string>);
	}
}
```

Server and Location redirect to a specific url using a specific status code.
```
server {
	redirect (IServer.IBlock._redirection_code<int>) (IServer.IBlock._redirection<std::string>);

	location /example/ {
		redirect (IServer.IBlock._redirection_code<int>) (IServer.IBlock._redirection<std::string>);
	}
}
```

Server and Location can specify a maximum size for the request body.
```
server {
	body_limit (IServer.IBlock._body_limit<int>);

	location /example/ {
		body_limit (ILocation.IBlock._body_limit<int>);
	}
}
```

Server and Location can allow specific methods.
```
server {
	allowed_methods (IServer.IBlock._allowed_methods<bool[]>);

	location /example/ {
		allowed_methods (ILocation.IBlock._allowed_methods<bool[]>);
	}
}
```

Server and location can define CGI run for specific files extension.
```
server {
	cgi	(IServer.IBlock._cgi<std::map<std::string ext, std::string path>>)

	location /example/ {
		cgi	(ILocation.IBlock._cgi<std::map<std::string ext, std::string path>>)
	}
}
```
