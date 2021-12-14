# Reach server 2 with curl
printf "Reach server 2 with curl\n"
curl --resolve webserv.com:8000:127.0.0.1 http://webserv.com:8000

# Reach server 1 with curl
printf "\n\n\nReach server 1 with curl\n"
curl --resolve example.com:8000:127.0.0.1 http://example.com:8000

# Reach default server (server 1) with curl
printf "\n\n\nReach default server (server 1) with curl\n"
curl http://localhost:8000/
