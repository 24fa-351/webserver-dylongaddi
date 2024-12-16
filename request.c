#include "request.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool read_request_line(Request *req, int fd);
bool read_headers(Request *req, int fd);
bool read_body(Request *req, int fd);

Request *request_read_from_fd(int fd) {
  printf("Reading request from fd %d\n", fd);

  Request *req = malloc(sizeof(Request));
  if (read_request_line(req, fd) == false) {
    printf("Failed to read request Lline\n");
    request_free(req);
    return NULL;
  }
  if (read_headers(req, fd) == false) {
    printf("Failed to read headers\n");
    request_free(req);
    return NULL;
  }
  if (read_body(req, fd) == false) {
    printf("Failed to read body\n");
    request_free(req);
    return NULL;
  }

  return req;
}

void request_print(Request *req) {
  printf("vvv Request vvv\n");

  if (req->method) {
    printf("Method: %s\n", req->method);
  }

  if (req->path) {
    printf("path: %s\n", req->path);
  }

  if (req->version) {
    printf("version: %s\n", req->version);
  }
  printf("^^^ Request ^^^");
}

void request_free(Request *req) {
  printf("freeing request\n");
  if (req->method) {
    free(req->method);
  }

  if (req->path) {
    free(req->path);
  }

  if (req->version) {
    free(req->version);
  }

  free(req);
}

char *read_line(int fd) {
  printf("Reading line from td %d\n", fd);
  return NULL;
}

bool read_request_line(Request *req, int fd) {
  printf("Reading request line\n");
  char *line = read_line(fd);
  if (line == NULL) {
    return false;
  }
  req->method = malloc(strlen(line) + 1);
  req->path = malloc(strlen(line) + 1);
  req->version = malloc(strlen(line) + 1);

  int length_parsed;
  int num_parsed;

  num_parsed = sscanf(
      line, "%s %s %s%n", req->method, req->path, req->version,
      &length_parsed); //%n saves num of characters parsed into length parsed
  if (num_parsed != 3 || length_parsed != (int)strlen(line)) {
    printf("Failed to parse request line\n");
    free(line);
    return false;
  }

  if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "POST") != 0) {
    printf("Invalid method: %s\n", req->method);
    free(line);
    return false;
  }

  return true;
}

bool read_headers(Request *req, int fd) {
  char *line;
  while ((line = read_line(fd)) != NULL && strcmp(line, "\r\n") != 0) {
    free(line);
  }
  return true;
}
bool read_body(Request *req, int fd) {
  if (req->method && strcmp(req->method, "POST") == 0) {
    req->body = malloc(req->body_length);
    read(fd, req->body, req->body_length);
  }
  return true;
}
