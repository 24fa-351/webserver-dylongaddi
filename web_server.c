#include "http_message.h"
#include "request.h"
#include "routes.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// based on bruce's python implementation**

#define HOST "127.0.0.1" // Localhost
#define PORT 80          // Port to listen on
#define BUFFER_SIZE 1024 // Buffer size for receiving data
#define LISTEN_BACKLOG 5

int total_requests = 0;
int total_bytes_received = 0;
int total_bytes_sent = 0;

void server_dispatch_request(Request *req, int sock_fd) {
  if (strncmp(req->path, "/static", 7) == 0) {
    handle_static(sock_fd, req->path);
  } else if (strcmp(req->path, "/stats") == 0) {
    handle_stats(sock_fd);
  } else if (strncmp(req->path, "/calc", 5) == 0) {
    handle_calc(sock_fd, req->path);
  } else {
    char *response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    write(sock_fd, response, strlen(response));
  }
}

// input ptr freed by this function
void handle_connection(int *sock_fd_ptr) {
  int sock_fd = *sock_fd_ptr;
  free(sock_fd_ptr);

  printf("Handling connection on %d\n", sock_fd);

  while (1) {
    Request *req = request_read_from_fd(sock_fd);

    if (req == NULL) {
      break;
    }
    request_print(req);
    server_dispatch_request(req, sock_fd);
    request_free(req);
  }
  printf("Done with connection %d\n", sock_fd);
  close(sock_fd);
}

int main(int argc, char *argv[]) {
  int port = PORT;
  if (argc == 2 & !strcmp(argv[1], "-")) {
    printf("Reading ONE request from stdin\n");
    Request *req = request_read_from_fd(0);

    if (req = NULL) {
      printf("Failed to read request\n");
      exit(1);
    }
    request_print(req);
    request_free(req);
    exit(0);
  }

  if (argc == 2 && !strcmp(argv[1], "--handle")) {
    printf("Handling ONE connection from stdin\n");
    int *sock_fd = malloc(sizeof(int));
    *sock_fd = 0;
    handle_connection(sock_fd);
    printf("Done handling connection\n");
    exit(0);
  }

  if (argc > 1) {
    port = atoi(argv[1]);
  }
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in socket_address;
  memset(&socket_address, '\0', sizeof(socket_address));
  socket_address.sin_family = AF_INET;
  socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
  socket_address.sin_port = htons(PORT);

  int returnval;

  returnval = bind(socket_fd, (struct sockaddr *)&socket_address,
                   sizeof(socket_address));

  if (returnval < 0) {
    perror("bind");
    return 1;
  }

  returnval = listen(socket_fd, LISTEN_BACKLOG);

  struct sockaddr_in client_address;
  socklen_t client_address_len = sizeof(client_address);

  while (1) {
    pthread_t thread;
    int *client_fd_buf = 0;

    *client_fd_buf = accept(socket_fd, (struct sockaddr *)&client_address,
                            &client_address_len);
    if (*client_fd_buf > 0) {
      printf("accepted connection on %d\n", *client_fd_buf);

      pthread_create(&thread, NULL, (void *(*)(void *))handle_connection,
                     (void *)client_fd_buf);
    }
  }

  close(socket_fd);
  return 0;
}
