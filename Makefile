# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g

# Executable name
TARGET = web_server

# Source files
SRCS = web_server.c request.c http_message.c routes.c

# Object files (replace .c with .o in the SRCS list)
OBJS = $(SRCS:.c=.o)

# Header files (not strictly needed, but useful for dependencies)
HEADERS = request.h http_message.h routes.h

# Default target to build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to build object files from source files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Run the server with a default port of 8080
run: $(TARGET)
	./$(TARGET) 8080
