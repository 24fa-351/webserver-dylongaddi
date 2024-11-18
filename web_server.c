#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "http_message.h"

#define PORT 80       // Port to listen on
#define LISTEN_BACKLOG 5


//input ptr freed by this function
void handle_connection(int* sock_fd_ptr)
{
  int sock_fd = *sock_fd_ptr;
  free(sock_fd_ptr);

  printf("Handling connection on %d\n", sock_fd);

  char buffer[1024];
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
    http_client_message_free(http_msg);
  }
  printf("Done with connection %d\n", sock_fd);
  close(sock_fd);
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

  if (returnval < 0) {
    perror("bind");
    return 1;
  }

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
