#ifndef REQUEST_H:
#define REQUEST_H

typedef struct
{
  char* method;

  char* path;

  char* version;
} Request;

Request* request_read_fron_fd(int fd);

void request_print(Request* req);

void request_free(Request* req);

#endif
