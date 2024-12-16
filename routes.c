#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "routes.h"

void handle_static(int sock_fd, const char* path) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "static%s", path + 7);

    FILE* file = fopen(filepath, "rb");
    if (!file) {
        char* response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        write(sock_fd, response, strlen(response));
        return;
    }

    char response[] = "HTTP/1.1 200 OK\r\n\r\n";
    write(sock_fd, response, strlen(response));

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(sock_fd, buffer, bytes_read);
    }

    fclose(file);
}

void handle_stats(int sock_fd) {
    extern int total_requests;
    extern int total_bytes_received;
    extern int total_bytes_sent;

    char response[512];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
             "<html><body>"
             "<h1>Server Stats</h1>"
             "<p>Total requests: %d</p>"
             "<p>Total bytes received: %d</p>"
             "<p>Total bytes sent: %d</p>"
             "</body></html>",
             total_requests, total_bytes_received, total_bytes_sent);

    write(sock_fd, response, strlen(response));
}

void handle_calc(int sock_fd, const char* path) {
    int a = 0, b = 0;
    sscanf(path, "/calc?a=%d&b=%d", &a, &b);
    int sum = a + b;

    char response[256];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
             "Result: %d", sum);
    write(sock_fd, response, strlen(response));
}
