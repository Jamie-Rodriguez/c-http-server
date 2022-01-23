// Modified from:
// https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
#include "http-parsing.h"
#include <netinet/in.h> // INADDR_ANY, sockaddr_in
#include <stdio.h>      // perror(), printf()
#include <stdlib.h>     // exit(), EXIT_FAILURE
#include <string.h>     // memset(), strlen()
#include <sys/socket.h> // socket(), AF_INET, SOCK_STREAM, socklen_t, sockaddr
#include <time.h>       // time()
#include <unistd.h>     // read(), write(), close()


#define PORT 8080


int main(const int argc, char const *argv[]) {
        const int server_fd = socket(AF_INET, SOCK_STREAM, 0);

        if (server_fd == 0) {
                perror("In socket");
                exit(EXIT_FAILURE);
        }

        const struct sockaddr_in address = { .sin_family = AF_INET,
                                             .sin_addr.s_addr = INADDR_ANY,
                                             .sin_port = htons(PORT) };
        memset((void *) address.sin_zero, '\0', sizeof address.sin_zero);

        const socklen_t addr_len = sizeof(address);

        if (bind(server_fd, (struct sockaddr *) &address, addr_len) < 0) {
                perror("In bind");
                exit(EXIT_FAILURE);
        }

        const int backlog_size = 10;
        if (listen(server_fd, backlog_size) < 0) {
                perror("In listen");
                exit(EXIT_FAILURE);
        }


        while (1) {
                printf(
                    "\n++++++++++ Waiting for new connection ++++++++++++\n\n");

                // accept() blocks thread and waits for an incoming connection
                const int socket = accept(server_fd,
                                          (struct sockaddr *) &address,
                                          (socklen_t *) &addr_len);

                if (socket < 0) {
                        perror("In accept");
                        exit(EXIT_FAILURE);
                }

                char buffer[MAX_REQUEST_LEN] = { 0 };
                const long bytes_read = read(socket, buffer, sizeof(buffer));

                printf("Incoming request: \n\n");
                printf("%s\n", buffer);


                const char firstLine[MAX_REQUEST_LINE_LEN] = { 0 };
                getFirstLineFromHttpRequest(buffer,
                                            firstLine,
                                            sizeof(firstLine));
                const char path[MAX_REQUEST_LINE_LEN] = { 0 };
                getHttpUriFromRequestLine(firstLine, path, sizeof(path));

                unsigned int statusCode = 0;
                enum mimeType mime = 0;
                char body[64] = { 0 };
                // Kubernetes readiness probe endpoint
                if (strcmp(path, "/readyz") == 0) {
                        statusCode = 200;
                        mime = JSON;
                        memccpy(body,
                                "{\"success\":\"true\"}",
                                '\0',
                                sizeof(body));
                } else {
                        statusCode = 404;
                        mime = PLAIN_TEXT;
                        memccpy(body, "Not found.", '\0', sizeof(body));
                }

                const response response = { .status = statusCode,
                                            .body = body,
                                            .bodyContentLen = strlen(body),
                                            .time = time(0),
                                            .mime = mime };

                char output_buffer[500] = { 0 };
                createResponse(response, output_buffer, sizeof(output_buffer));

                printf("Response: \n\n");
                printf("%s\n", output_buffer);

                write(socket, output_buffer, strlen(output_buffer));

                printf("------------------Response sent-------------------\n");

                close(socket);
        }
}
