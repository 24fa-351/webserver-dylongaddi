#ifndef ROUTES_H
#define ROUTES_H

void handle_static(int sock_fd, const char* path);
void handle_stats(int sock_fd);
void handle_calc(int sock_fd, const char* path);

#endif
