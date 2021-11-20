#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <csignal>
#include <iostream>

#define MAX_EV 8192

bool	_alive = true;

static void	__handler(int sig) {
	(void)sig;
	_alive = false;
}

bool	signal_alive() {
	return _alive;
}

void	signal_setup(void) {
	signal(SIGINT, __handler);
	signal(SIGTERM, __handler);
}

void	signal_reset() {
	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGTERM, SIG_DFL);
}

int main(void) {
	struct epoll_event event = {0};
	struct epoll_event events[MAX_EV];

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(server_fd, F_SETFL, O_NONBLOCK);
	int _true = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &_true, sizeof(_true));

    
	bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
	listen(server_fd, MAX_EV);
	int epfd = epoll_create1(0);

	event.data.fd = server_fd;
	event.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &event);

	int nfds, i;
	while (signal_alive()) {
		nfds = epoll_wait(epfd, events, MAX_EV, 0);
		for (i = 0; i < nfds; i++) {
			if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP) {
				close(events[i].data.fd);
				continue;
			} else if (server_fd == events[i].data.fd) {
				std::cout << "accept" << std::endl;
				struct sockaddr in_addr = {0};
				socklen_t in_len = {0};

				int client_fd = accept(server_fd, &in_addr, &in_len);
				fcntl(client_fd, F_SETFL, O_NONBLOCK);
				event.data.fd = client_fd;
				event.events = EPOLLIN;
				epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
				continue ;
			} else if (events[i].events & EPOLLIN) {
				std::cout << "read" << std::endl;
				char buf[1024];

				int n = read(events[i].data.fd, buf, sizeof(buf));
				event.events = EPOLLOUT;
				event.data.fd = events[i].data.fd;
				epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &event);
			} else if (events[i].events & EPOLLOUT) {
				std::cout << "request" << std::endl;
				char *head = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 12\r\n\r\nHello World!";
				send(events[i].data.fd, head, strlen(head), 0);
				event.events = EPOLLIN;
				event.data.fd = events[i].data.fd;
				// close(events[i].data.fd);
				epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &event);
			}
		}
	}
    epoll_ctl(epfd, EPOLL_CTL_DEL, server_fd, NULL);
	close(epfd);
	close(server_fd);

	return 0;
}