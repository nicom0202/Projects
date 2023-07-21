#include "response.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "asgn2_helper_funcs.h"

#define OK_STATUS_PHRASE_SIZE                    "3\0"
#define CREATED_STATUS_PHRASE_SIZE               "8\0"
#define BAD_REQUEST_STATUS_PHRASE_SIZE           "12\0"
#define FORBIDDEN_STATUS_PHRASE_SIZE             "10\0"
#define NOT_FOUND_STATUS_PHRASE_SIZE             "10\0"
#define INTERNAL_SERVER_ERROR_STATUS_PHRASE_SIZE "21\0"
#define NOT_IMPLEMENTED_STATUS_PHRASE_SIZE       "16\0"
#define VERSION_NOT_SUPPORTED_STATUS_PHRASE_SIZE "22\0"

void write_invalid_response(Response *response) {
    char *version = "HTTP/1.1 ";
    write(response->fd_socket, version, (int) strlen(version));
    //catch error here

    write(response->fd_socket, response->status_code, strlen(response->status_code));
    //catch error here

    char *space = " ";
    write(response->fd_socket, space, strlen(space));

    write(response->fd_socket, response->status_phrase, strlen(response->status_phrase));
    //catch error here

    char *str = "\r\nContent-Length: ";
    write(response->fd_socket, str, strlen(str));
    //catch error here

    //(**********)
    char *size;
    if (strcmp(response->status_phrase, "OK") == 0) {
        size = OK_STATUS_PHRASE_SIZE;
    }
    if (strcmp(response->status_phrase, "Created") == 0) {
        size = CREATED_STATUS_PHRASE_SIZE;
    }
    if (strcmp(response->status_phrase, "Bad Request") == 0) {
        size = BAD_REQUEST_STATUS_PHRASE_SIZE;
    }
    if (strcmp(response->status_phrase, "Forbidden") == 0) {
        size = FORBIDDEN_STATUS_PHRASE_SIZE;
    }
    if (strcmp(response->status_phrase, "Not Found") == 0) {
        size = NOT_FOUND_STATUS_PHRASE_SIZE;
    }
    if (strcmp(response->status_phrase, "Internal Server Error") == 0) {
        size = INTERNAL_SERVER_ERROR_STATUS_PHRASE_SIZE;
    }
    if (strcmp(response->status_phrase, "Not Implemented") == 0) {
        size = NOT_IMPLEMENTED_STATUS_PHRASE_SIZE;
    }
    if (strcmp(response->status_phrase, "Version Not Supported") == 0) {
        size = VERSION_NOT_SUPPORTED_STATUS_PHRASE_SIZE;
    }

    write(response->fd_socket, size, strlen(size));
    //catch error here
    //(**********)

    char *end_header = "\r\n\r\n";
    write(response->fd_socket, end_header, strlen(end_header));
    //catch error here

    write(response->fd_socket, response->status_phrase, strlen(response->status_phrase));
    //catch error here

    char *end = "\n";
    write(response->fd_socket, end, strlen(end));
    //catch error here
}

void write_valid_response(Response *response) {
    char *version = "HTTP/1.1 \0";
    write(response->fd_socket, version, (int) sizeof(version));
    //catch error here

    write(response->fd_socket, response->status_code, sizeof(response->status_code));
    //catch error here

    char *space = " ";
    write(response->fd_socket, space, sizeof(space));

    write(response->fd_socket, response->status_phrase, sizeof(response->status_phrase));
    //catch error here

    char *str = "\r\nContent-Length: \0";
    write(response->fd_socket, str, sizeof(str));
    //catch error here

    char *size = response->content_length;
    write(response->fd_socket, size, sizeof(size));
    //catch error here

    char *end_header = "\r\n\r\n\0";
    write(response->fd_socket, end_header, sizeof(end_header));
    //catch error here
}

void bad_request(int fd) {
    char *bad = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
    write_all(fd, bad, strlen(bad));
    //catch error here
}

void forbidden_request(int fd) {
    char *bad = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";
    write_all(fd, bad, strlen(bad));
    //catch error here
}

void set_opened_request(int fd) {
    char *opened = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";
    write_all(fd, opened, strlen(opened));
    //catch error here
}

void set_created_request(int fd) {
    char *created = "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n";
    write_all(fd, created, strlen(created));
    //catch error here
}

void get_not_found_request(int fd) {
    char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
    write_all(fd, not_found, strlen(not_found));
    //catch error here
}

void not_implemented_request(int fd) {
    char *not_implemented
        = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\nNot Implemented\n";
    write_all(fd, not_implemented, strlen(not_implemented));
    //catch error here
}

void version_not_supported_request(int fd) {
    char *version_not_supported
        = "HTTP/1.1 505 Version Not Supported\r\nContent-Length: 22\r\n\r\nVersion Not Supported\n";
    write_all(fd, version_not_supported, strlen(version_not_supported));
    //catch error here
}

void internal_server_error(int fd) {
    char *version_not_supported
        = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n";
    write_all(fd, version_not_supported, strlen(version_not_supported));
    //catch error here
}

void valid_get_response(int fd, char *filesize) {
    char *a = "HTTP/1.1 200 OK\r\nContent-Length: ";
    write_all(fd, a, strlen(a));
    //catch error here

    write_all(fd, filesize, strlen(filesize));
    //catch error here

    char *b = "\r\n\r\n";
    write_all(fd, b, strlen(b));
    //catch error here
}

void valid_put_created_response(int fd) {
    char *a = "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n";
    write_all(fd, a, strlen(a));
    //catch error
}

void valid_put_overwritten_response(int fd) {
    char *a = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";
    write_all(fd, a, strlen(a));
    //catch error
}

void empty_response(int fd) {
    char *a = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\n";
    write_all(fd, a, strlen(a));
    //catch error
}
