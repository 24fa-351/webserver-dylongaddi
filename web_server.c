#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "http_message.h"
//based on bruce's python implementation**

#define HOST "127.0.0.1" // Localhost
#define PORT 80       // Port to listen on
#define BUFFER_SIZE 1024 // Buffer size for receiving data
#define LISTEN_BACKLOG 5

int respond_to_http_client_message(int sock_fd, http_client_message_t *http_msg)
{
  char *response = "HTTP/1.1 200 OK\r\nContent-Length:-0\r\n\r\n";
  write(sock_fd, response, strlen(response));
  return 0;
}

//input ptr freed by this function
void handle_connection(int* sock_fd_ptr)
{
  int sock_fd = *sock_fd_ptr;
  free(sock_fd_ptr);

  printf("Handling connection on %d\n", sock_fd);

  char buffer[BUFFER_SIZE];
  while (1)
  {
    printf("Handling connection on %d\n", sock_fd);
    http_client_message_t *http_msg;
    http_read_result_t result;
    read_http_client_message(sock_fd, &http_msg, &result);
    if (result == BAD_REQUEST)
    {
      printf("Bad request\n");
      close(sock_fd);
      return;
    }
    else if (result == CLOSED_CONNECTION)
    {
      printf("Closed connection\n");
      close(sock_fd);
      return;
    }
    respond_to_http_client_message(sock_fd, http_msg);
  }
  printf("Done with connection %d\n", sock_fd);
}



int main(int argc, char* argv[])
{
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in socket_address;
  memset(&socket_address, '\0', sizeof(socket_address));
  socket_address.sin_family = AF_INET;
  socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
  socket_address.sin_port = htons(PORT);

  int returnval;

  returnval = bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address));

  returnval = listen(socket_fd, LISTEN_BACKLOG);

  struct sockaddr_in client_address;
  socklen_t client_address_len = sizeof(client_address);

  while (1)
  {
    pthread_t thread;
    int *client_fd_buf = 0;

    *client_fd_buf = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_len);
    if (*client_fd_buf > 0)
    {
      printf("accepted connection on %d\n", *client_fd_buf);

      pthread_create(&thread, NULL, (void *(*)(void *))handle_connection, (void *)client_fd_buf);
    }
  }

  close(socket_fd);
  return 0;
}
