#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char *success;
    char *uri_created;
    char *bad_request;
    char *forbidden;
    char *not_found;
    char *internal_server_error;
    char *not_implemented;
    char *version_not_supported;
} Status_Code;

struct Header_Field {
    char *key;
    char *value;
    struct Header_Field *next;
};

typedef struct {
    char *method;
    char *buffer;
    char *uri;
    char *version;
    int buf_size;
    int fd_socket;
    int end_of_header_field_pos;
    char *content_length;
    char *status_code;
    char *status_phrase;
    //size_t request_size;
    Status_Code *status_code_list;
    struct Header_Field *header_field;
} Request;

int parse_handler(char *, Request *);

int parse_method(Request *);
int check_method(Request *);

int parse_uri(Request *);
int check_uri(Request *);

int parse_version(Request *);
int check_version(Request *);

int parse_header(Request *);
int check_header_line(Request *);
