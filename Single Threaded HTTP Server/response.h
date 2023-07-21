#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
//#include "parse.h"

typedef struct {
    char *version;
    char *status_code;
    char *status_phrase;
    char *header_field_key;
    int head_field_value;
    int fd_socket;
    char *content_length;
} Response;

void write_invalid_response(Response *);
void write_valid_response(Response *);
void bad_request(int);
void forbidden_request(int);
void set_opened_request(int);
void set_created_request(int);
void get_not_found_request(int);
void not_implemented_request(int);
void version_not_supported_request(int);
void internal_server_error(int);
void valid_get_response(int, char *);
void valid_put_created_response(int);
void valid_put_overwritten_response(int);
void empty_response(int);
