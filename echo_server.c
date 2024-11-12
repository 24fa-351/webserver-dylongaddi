#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
//based on bruce's python implementation**

#define HOST "127.0.0.1" // Localhost
#define PORT 65432       // Port to listen on
#define BUFFER_SIZE 1024 // Buffer size for receiving data

// Define the function to handle each client connection
void handle_connection(int client_socket)
{
  printf("Connected by client\n");

  char buffer[BUFFER_SIZE];
  while (1)
  {
    // Receive data from the client
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0)
    {
      break; // Exit the loop if no data is received or if an error occurs
    }

    // Echo the received data back to the client
    send(client_socket, buffer, bytes_received, 0);
  }

  close(client_socket);
  printf("Client disconnected\n");
}

int main()
{
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addr.sin_port = htons(65432);

  bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
  listen(server_socket, 10);

  while (1)
  {
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket > 0)
    {
      handle_connection(client_socket);
    }
  }

  close(server_socket);
  return 0;
}
