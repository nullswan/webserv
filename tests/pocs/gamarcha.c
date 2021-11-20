#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
// #include <sys/time.h>
// #include <sys/types.h>
// #include <unistd.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <assert.h>

#define NOTNULL ((void*)!0)
#define PORT    8000

// int main(void) {

//     fd_set rfds;
//     struct timeval tv;
//     int retval;
//     bool breakCondition;

//     breakCondition = true;
//     while (breakCondition == true) {

//         /* watch stdin (fd 0) for waiting input */
//         FD_ZERO(&rfds);
//         FD_SET(0, &rfds);

//         /* for 5 seconds */
//         tv.tv_sec = 5;
//         tv.tv_usec = 0;

//         retval = select(1, &rfds, NULL, NULL, &tv);
//         /* tv is undefined after select call ! */

//         if (retval == -1) {
//             perror("select()");
//             exit(EXIT_FAILURE);
//         }
//         else if (retval == 0) {
//             breakCondition = false;
//         }
//         else if (retval != 0) {
//             /* FD_ISSET(0, &rfds) is true */
//             while (getchar() != '\n');
//         }
//     }

//     printf("Aucune données durant les 5 secondes\n");
//     exit(EXIT_SUCCESS);
// }

// int main(void) {

//     struct pollfd fds;
//     int retval;
//     size_t outputSize;
//     char output[4096 + 1], buf[4096 + 1];
//     bool breakCondition;

//     fds.fd = STDIN_FILENO;
//     fds.events = POLLIN;
//     outputSize = 0;
//     memset(output, 0, 4096 + 1);
//     memset(buf, 0, 4096 + 1);
//     breakCondition = true;
//     while (breakCondition == true) {

//         retval = poll(&fds, (nfds_t)1, 3000);

//         if (retval == -1) {
//             perror("poll()");
//             exit(EXIT_FAILURE);
//         }
//         else if (retval == 0) {
//             breakCondition = false;
//         }

//         else if (fds.revents != 0) {
//             if (fds.revents & POLLIN) {
//                 const ssize_t bufSize = read(0, buf, 4096);
//                 if (bufSize == -1) {
//                     perror("read()");
//                     exit(EXIT_FAILURE);
//                 }
//                 buf[bufSize - 1] = '\0';
//                 if (outputSize != 0) {
//                     strcpy(output + outputSize, " ");
//                     outputSize++;
//                 }
//                 strcpy(output + outputSize, buf);
//                 outputSize += bufSize - 1;
//             }
//             else {
//                 perror("poll()");
//                 exit(EXIT_FAILURE);
//             }
//         }
//     }

//     printf("data: %s\n", output);
//     exit(EXIT_SUCCESS);
// }

void    closeClients(int *clientfds) {

    for (size_t i = 0; i < 128; i++) {
        if (clientfds[i] != 0) {
            close(clientfds[i]);
            clientfds[i] = 0;
        }
    }
    printf("Server shutdown. Port %d released.\n", PORT);
}

int    handleStdin(int *clientfds) {

    static char buf[4096 + 1];

    const ssize_t bufSize = read(STDIN_FILENO, buf, 4096);
    if (bufSize == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
    }
    else if (bufSize == 0) {
        closeClients(clientfds);
        return 1;
    }
    else {
        buf[bufSize - 1] = '\0';
        if (buf[bufSize - 2] == '\r') {
            buf[bufSize - 2] = '\0';
        }
        if (strcmp(buf, "exit") == 0) {
            closeClients(clientfds);
            return 1;
        }
    }
    return 0;
}

void    handleConnexion(int sockfd, int epfd, int *clientfds, struct sockaddr_in *addr, socklen_t *addrLen, struct epoll_event *ev) {

    int clientfd = accept(sockfd, (struct sockaddr *)addr, (socklen_t *)addrLen);
    if (clientfd == -1) {
        perror("accept()");
        exit(EXIT_FAILURE);
    }

    ev->data.fd = clientfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, ev) == -1) {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }

    size_t i = 0;
    while (i < 128 && clientfds[i] != 0) {
        i++;
    }
    clientfds[i] = clientfd;

    printf("New connexion, client %lu\n", i + 1);

    // char *msg = "ECHO WEBSERV@0.0.7 \\\\\n";
    // const ssize_t msgLen = strlen(msg);
    // if (send(clientfd, msg, msgLen, 0) != msgLen) {
    //     perror("send()");
    //     exit(EXIT_FAILURE);
    // }

    // clientfds[i]  = 0;
    // close(clientfd);
}

void    handleRequest(int fd, int client, int *clientfds, __attribute__((unused)) char *buf) {

    static char *content =
        "<html>\n"
        "   <body>\n"
        "       <div>\n"
        "           Hello World!\n"
        "       </div>\n"
        "   </body>\n"
        "</html>";
    const size_t contentLen = strlen(content);

    char res[4096];
    const ssize_t resLen = sprintf(res, "%s\r\n%s\r\n%s%lu\r\n\r\n%s",
        "HTTP/1.1 200 OK",
        "Content-Type: text/html; charset=utf-8",
        "Content-Length: ", contentLen,
        content
    );

    if (send(fd, res, resLen, 0) != resLen) {
        perror("send()");
        exit(EXIT_FAILURE);
    }

    clientfds[client]  = 0;
    close(fd);
    printf("Client %lu is disconnected\n", client + 1);
}

void    handleInput(int fd, int *clientfds, char *output) {

    static size_t outputSize;
    static char buf[4096 + 1];

    size_t client = 0;
    while (client < 128 && fd != clientfds[client]) {
        client++;
    }

    ssize_t bufSize = read(fd, buf, 4096);
    if (bufSize == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
    }
    else if (buf[0] == 4) {
        
        clientfds[client] = 0;
        close(fd);
        printf("Client %lu is disconnected\n", client + 1);
    }
    else {

        buf[--bufSize] = '\0';
        if (buf[bufSize - 1] == '\r') {
            buf[--bufSize] = '\0';
        }
        if (outputSize != 0) {
            strcpy(output + outputSize, ",\n");
            outputSize += 2;
        }
        strcpy(output + outputSize, buf);
        outputSize += bufSize;
        printf("buf value: [%s], bufSize: %ld, outputSize: %ld\n", buf, bufSize, outputSize);
        handleRequest(fd, client, clientfds, buf);
    }
}

int main(void) {

    static struct epoll_event ev[16];
    struct sockaddr_in addr;
    socklen_t addrLen;
    int sockfd, epfd, nfds, optval, clientfds[128] = {0};
    bool breakCondition;
    static char output[4096 * 4 + 1];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
        perror("setsockopt()");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    addrLen = sizeof(addr);
    if (bind(sockfd, (struct sockaddr *)&addr, addrLen) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) == -1) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    epfd = epoll_create(!0);
    if (epfd == -1) {
        perror("epoll_create()");
        exit(EXIT_FAILURE);
    }

    ev->events = EPOLLIN | EPOLLPRI; /* EPOLLERR | EPOLLHUP are auto*/

    ev->data.fd = STDIN_FILENO;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, ev) == -1) {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }

    ev->data.fd = sockfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, ev) == -1) {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }

    breakCondition = true;
    while (breakCondition == true) {

        nfds = epoll_wait(epfd, ev, sizeof(ev) / sizeof(ev[0]), -1);
        if (nfds == -1) {
            perror("epoll_wait()");
            exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < (size_t)nfds; i++) {
            const int fd = ev[i].data.fd;
            if (fd == STDIN_FILENO) {
                if (handleStdin(clientfds) == 1) {
                    breakCondition = false;
                }
            }
            else if (fd == sockfd) {
                handleConnexion(fd, epfd, clientfds, &addr, &addrLen, ev);
            }
            else {
                handleInput(fd, clientfds, output);
            }
        }
    }

    close(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_DEL, STDIN_FILENO, NULL);
    close(epfd);
    printf("data:\n%s\n", output);
    exit(EXIT_SUCCESS);
}
