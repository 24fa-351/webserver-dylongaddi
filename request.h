#ifndef REQUEST_H
#include <stdbool.h>
#define REQUEST_H

typedef struct {
  char *method;
  char *path;
  char *version;
  char *body;
  int body_length;
} Request;

Request *request_read_from_fd(int fd);

void request_print(Request *req);

void request_free(Request *req);

bool read_headers(Request *req, int fd);

bool read_body(Request *req, int fd);

#endif
