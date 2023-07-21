#include "parse.h"
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

//#define METHOD_REGEX    "^([a-zA-Z]+)( )"
#define METHOD_REGEX       "^([a-zA-Z]){1,8}( )"
#define URI_REGEX          "/(/?[a-zA-Z0-9_.-])*( )"
#define VERSION_REGEX      "(HTTP/[0-9].[0-9]\r\n)"
#define HEADER_LINE_REGEX  "[^\r\n]+\r\n"
#define HEADER_KEY_PATTERN "^[a-zA-Z0-9.-]+: "

#define OK_STATUS_PHRASE                    "OK"
#define CREATED_STATUS_PHRASE               "Created"
#define BAD_REQUEST_STATUS_PHRASE           "Bad Request"
#define FORBIDDEN_STATUS_PHRASE             "Forbidden"
#define NOT_FOUND_STATUS_PHRASE             "Not Found"
#define INTERNAL_SERVER_ERROR_STATUS_PHRASE "Internal Server Error"
#define NOT_IMPLEMENTED_STATUS_PHRASE       "Not Implemented"
#define VERSION_NOT_SUPPORTED_STATUS_PHRASE "Version Not Supported"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FUNCTION: parse_handler()
//PARAMETERS:
//RETURNS: 0 if valid request
//         1 if bad request
//         2 internal server error
//         3 not implemented
//         4 version no supported
//         5
//PURPOSE:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int parse_handler(char *buffer, Request *request) {

    if (buffer == NULL) {
        return 0;
    }

    //STEP 1: need to see if "\r\n\r\n" exists in buffer
    //          (a) If not, then return invalid request
    //          (b) If yes, then find that position and store it for later b/c printing contents will need it
    //>>>>>>> Steps to use regex <<<<<<<

    //step(1) : declare regex_t, regmatch_t, & delimiter variables
    regex_t regex;
    regmatch_t match; //Substring position
    char *end_of_header_field = "\r\n\r\n";

    //step(2) : compile the regular expression pattern
    int ret = regcomp(&regex, end_of_header_field, REG_EXTENDED);
    if (ret != 0) {
        /*
        request->status_code = request->status_code_list->internal_server_error;
        request->status_phrase = INTERNAL_SERVER_ERROR_STATUS_PHRASE;
        */
        //regfree(&regex);
        return 2;
    }

    //step(3) : Execute the regular expression search
    ret = regexec(
        &regex, buffer, 1, &match, 0); //if equal to 0 then found at position match, else not found

    if (ret == REG_NOMATCH) {
        /*
        request->status_code = request->status_code_list->bad_request;
        request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
        */
        regfree(&regex);
        return 1;
    }
    if (ret != 0) { //this was not found therefore it is an invalid request
        /*
        request->status_code = request->status_code_list->internal_server_error;
        request->status_phrase = INTERNAL_SERVER_ERROR_STATUS_PHRASE;
        */
        regfree(&regex);
        return 2;
    }

    //now put the position of "\r\n\r\n" into request->end_of_header_field_pos
    request->end_of_header_field_pos
        = match.rm_eo; //this puts the position at the end of the delimiter

    regfree(&regex);

    //now send to mini parsing functions to fill in request struct
    //~~~~~~method checking~~~~~~~~
    int valid_method = -1;
    if (request != NULL) {
        valid_method = parse_method(request);
    }
    /*
    if(valid_method == 1){
        return 1;
    }
    */
    if (valid_method != 0) {
        return valid_method;
    }
    //~~~~~~uri checking~~~~~~~~
    int valid_uri = parse_uri(request);
    /*
    if(valid_uri == 1){
        return 1;
    }
    */
    if (valid_uri != 0) {
        return valid_uri;
    }
    //~~~~~~version checking~~~~~~~~
    int valid_version = parse_version(request);
    /*
    if(valid_version == 1){
        return 1;
    }
    */
    if (valid_version != 0) {
        return valid_version;
    }
    //HERE NEED TO CHECK IF BUFFER POINTER IS POINTING AT THE "\r\n" for no

    //~~~~~~header checking~~~~~~~~
    int valid_header_field = parse_header(request);
    /*
    if(valid_header_field == 1){
        return 1;
    }
    */
    if (valid_header_field != 0) {
        return valid_header_field;
    }
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FUNCTION: parse_method()
//PARAMETERS: struct Request*
//RETURNS: 0 if found and parsed
//         1 if not found OR not valid (check_method will update request->status_code)
//PURPOSE: To parse out method in request-line
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int parse_method(Request *request) {
    regex_t re;
    regmatch_t match;
    int rc = 0;

    rc = regcomp(&re, METHOD_REGEX, REG_EXTENDED);
    if (rc != 0) {
        /*
            request->status_code = request->status_code_list->internal_server_error;
            request->status_phrase = INTERNAL_SERVER_ERROR_STATUS_PHRASE;
            */
        regfree(&re);
        return 2;
    }

    rc = regexec(&re, (char *) request->buffer, 1, &match, 0);

    if (rc == REG_NOMATCH) { //no match in expression so bad request
        /*
            request->status_code = request->status_code_list->bad_request;
            request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
            */
        regfree(&re);
        return 1;
    }
    if (rc != 0) { //error occured
        /*
            request->status_code = request->status_code_list->internal_server_error;
            request->status_phrase = INTERNAL_SERVER_ERROR_STATUS_PHRASE;
            */
        regfree(&re);
        return 2;
    }

    request->method = malloc(match.rm_eo - match.rm_so + 1); //took out +1
    memcpy(request->method, request->buffer + match.rm_so, match.rm_eo - match.rm_so - 1);
    request->method[match.rm_eo - match.rm_so - 1] = '\0';

    request->buffer
        += match.rm_eo - match.rm_so; //UPDATING BUFFER TO POITNT TO THE END OF METHOD!!!!

    regfree(&re); //SEE WHAT THIS RETURNS AND CHECK FOR AN ERROR!!!!!

    int rc_check = check_method(request);
    /*
        if(rc_check == 1){//invalid method
            return 1;
        }
        */
    if (rc_check != 0) {
        return rc_check;
    }
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FUNCTION: check_method()
//PARAMETERS: struct Request*
//RETURNS: 0 if valid method name
//         1 if invalid (will update request->status_code for what error happened)
//PURPOSE: to check if parse_method parses a valid method
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int check_method(Request *request) {
    char *r = request->method;
    if (r == NULL) {
        return 1;
    }

    if (strlen(r) > 8) {
        /*
        request->status_code = request->status_code_list->bad_request;
        request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
        */
        return 1; //not a valid method
    }

    if (strcmp(r, "GET") != 0 && strcmp(r, "PUT") != 0) {
        /*
        request->status_code = request->status_code_list->not_implemented;
        request->status_phrase = NOT_IMPLEMENTED_STATUS_PHRASE;
        */
        return 3; //not implemented yet
    }
    /*
    request->status_code = request->status_code_list->success;
    request->status_phrase = OK_STATUS_PHRASE;
    */
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FUNCTION: parse_uri()
//PARAMETERS: struct Request*
//RETURNS: 0 if valid method name
//         1 if not found OR not valid (check_uri will update request->status_code)
//PURPOSE:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int parse_uri(Request *request) {
    regex_t re;
    regmatch_t match;
    int rc = 0;

    if ((rc = regcomp(&re, URI_REGEX, REG_EXTENDED)) != 0) {
        /*
            request->status_code = request->status_code_list->internal_server_error;
            request->status_phrase = INTERNAL_SERVER_ERROR_STATUS_PHRASE;
            */
        regfree(&re);
        return 2;
    }

    rc = regexec(&re, (char *) request->buffer, 1, &match, 0);

    if (rc == REG_NOMATCH) { //no match in expression so bad request
        /* 
            request->status_code = request->status_code_list->bad_request;
            request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
            */
        regfree(&re);
        return 1;
    }
    if (rc != 0) { //this was not found therefore it is an invalid request
        /*
            request->status_code = request->status_code_list->internal_server_error;
            request->status_phrase = INTERNAL_SERVER_ERROR_STATUS_PHRASE;
            */
        regfree(&re);
        return 2;
    }

    request->uri = malloc(match.rm_eo - match.rm_so + 1);
    memcpy(request->uri, request->buffer + match.rm_so + 1, match.rm_eo - match.rm_so - 2);
    request->uri[match.rm_eo - match.rm_so - 2] = '\0';

    request->buffer += match.rm_eo - match.rm_so; //UPDATING BUFFER TO POITNT TO THE END OF URI!!!!

    regfree(&re); //SEE WHAT THIS RETURNS AND CHECK FOR AN ERROR!!!!!

    int rc_check = check_uri(request);
    /*
        if(rc_check == 1){//invalid method
            return 1;
        }
        */
    //    printf("----%s-----\n",request->uri);
    if (rc_check != 0) {
        return rc_check;
    }
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FUNCTION: check_uri()
//PARAMETERS:
//RETURNS: valid uri returns 0
//         invalid returns 1
//PURPOSE:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int check_uri(Request *request) {
    if (request->uri == NULL) {
        return 1;
    }
    if (strlen(request->uri) < 2
        || strlen(request->uri)
               > 64) { //adding a plus one because i took out the / in the request->uri
        /*
        request->status_code = request->status_code_list->bad_request;
        request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
        */
        return 1;
    }
    //(*)NOTE: NEED TO OPEN FILE AFTER PARSING IS DONE, PARSING IS JUST FOR PARSING
    /*
    request->status_code = request->status_code_list->success;
    request->status_phrase = OK_STATUS_PHRASE;
    */
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FUNCTION: parse_version()
//PARAMETERS:
//RETURNS: 0 if found and parsed
//         1 if not found OR not valid (check_method will update request->status_code)
//PURPOSE:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int parse_version(Request *request) {
    regex_t re;
    regmatch_t match;
    int rc = 0;

    if ((rc = regcomp(&re, VERSION_REGEX, REG_EXTENDED)) != 0) {
        /*
            request->status_code = request->status_code_list->internal_server_error;
            request->status_phrase = INTERNAL_SERVER_ERROR_STATUS_PHRASE;
            */
        regfree(&re);
        return 2;
    }
    rc = regexec(&re, (char *) request->buffer, 1, &match, 0);

    if (rc == REG_NOMATCH) { //no match in expression so bad request
        /* 
            request->status_code = request->status_code_list->bad_request;
            request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
            */
        regfree(&re);
        return 1;
    }
    if (rc != 0) { //error occured
        /*
            request->status_code = request->status_code_list->internal_server_error;
            request->status_phrase = INTERNAL_SERVER_ERROR_STATUS_PHRASE;
            */
        regfree(&re);
        return 2;
    }

    request->version = malloc(match.rm_eo - match.rm_so + 1);
    memcpy(request->version, request->buffer + match.rm_so, match.rm_eo - match.rm_so - 2);
    request->version[match.rm_eo - match.rm_so - 2] = '\0';

    request->buffer
        += match.rm_eo - match.rm_so; //UPDATING BUFFER TO POITNT TO THE END OF VERSION!!!!

    regfree(&re); //SEE WHAT THIS RETURNS AND CHECK FOR AN ERROR!!!!!

    int rc_check = check_version(request);
    /*
        if(rc_check == 1){//invalid method
            return 1;
        }
        */
    if (rc_check != 0) {
        return rc_check;
    }
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FUNCTION: check_version()
//PARAMETERS:
//RETURNS: 0 if valid method name
//         1 if invalid (will update request->status_code for what error happened)
//PURPOSE:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int check_version(Request *request) {
    if (request->version == NULL) {
        return 4;
    }
    if (strcmp(request->version, "HTTP/1.1") != 0) {
        /*
        request->status_code = request->status_code_list->version_not_supported;
        request->status_phrase = VERSION_NOT_SUPPORTED_STATUS_PHRASE;
        */
        return 4;
    }
    /*
    request->status_code = request->status_code_list->success;
    request->status_phrase = OK_STATUS_PHRASE;
    */
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FUNCTION: parse_header_field()
//PARAMETERS:
//RETURNS: 0 if found and parsed
//         1 if not found OR not valid (check_header_field will update request->status_code)
//PURPOSE:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int parse_header(Request *request) {

    regex_t header_line_re;
    regex_t buffer_end_pos;
    regmatch_t match;
    int buffer_index = 0;
    int buffer_end;
    int rc;

    //find the end position of the buffer
    char *end_of_header_field = "\r\n\r\n";

    int ret = regcomp(&buffer_end_pos, end_of_header_field, 0);
    if (ret != 0) {
        return 2;
    }

    ret = regexec(&buffer_end_pos, request->buffer, 1, &match,
        0); //if equal to 0 then found at position match, else not found

    if (ret == REG_NOMATCH) {
        //we already checked if '\r\n\r\n' exists before and it does, this checks if the header-field exists or not
        return 0;
    }
    if (ret != 0) { //this was not found therefore it is an invalid request
        return 2;
    }

    buffer_end = match.rm_eo; //buffer end holds the last index in the buffer

    //compile
    rc = regcomp(&header_line_re, HEADER_KEY_PATTERN, REG_EXTENDED);
    if (rc != 0) {
        return 2;
    }
    rc = regcomp(&header_line_re, HEADER_LINE_REGEX, REG_EXTENDED);
    if (rc != 0) {
        return 2;
    }

    //while-loop through all the
    while (request->buffer[0] != '\r' && buffer_index < buffer_end) {

        //(*********  KEY  ***********)
        rc = regexec(&header_line_re, (char *) request->buffer, 1, &match, 0);

        //error catching
        if (rc == REG_NOMATCH) { //no match in expression so bad request
            return 1;
        }
        if (rc != 0) { //error occured
            return 2;
        }

        int key_length = (int) match.rm_eo - (int) match.rm_so;

        char *header_line
            = strndup(request->buffer, key_length - 2); //copying value and cutting off "\r\n"

        //now strtok this header_line
        char *pos_colon = strchr(header_line, ':');
        if (pos_colon == NULL) {
            return 1;
        }
        char *key = strndup(header_line, pos_colon - header_line);

        char *pos_space = strchr(header_line, ' ');
        if (pos_space == NULL) {
            return 1;
        }
        //make sure the space and colon are next to each other
        if (pos_space - pos_colon != 1) {
            return 1;
        }

        //header_line = header_line + strlen(key) + 2;
        char *value = strndup(pos_space + 1, strlen(header_line) - strlen(key) + 2);

        //now create a header-field struct and have request point to it
        struct Header_Field *hf = malloc(sizeof(struct Header_Field));
        hf->key = key;
        hf->value = value;
        hf->next = NULL;

        //now add new node to the front of the link list
        struct Header_Field *temp = request->header_field;
        request->header_field = hf;
        hf->next = temp;
        temp = NULL;

        //updating buffer pos in request
        request->buffer = &request->buffer[key_length];

        //update buffer_index
        buffer_index = buffer_index + key_length;
    }
    request->buffer = request->buffer + 2;
    int check = check_header_line(request);
    if (check != 0) {
        return check;
    }
    regfree(&header_line_re); //SEE WHAT THIS RETURNS AND CHECK FOR AN ERROR!!!!!
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FUNCTION: check_each_header_line()
//PARAMETERS:
//RETURNS: 0 all header lines are correct
//         1 if invalid (will update request->status_code for what error happened)
//PURPOSE: this also captures Content-Length's value, if no key contains content length then request->content lenght is set to -1
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int check_header_line(Request *request) {
    struct Header_Field *curr = request->header_field;
    bool content_length_found = false;
    while (curr != NULL) {
        //key needs to be 1 < x < 128
        //value needs to be x < 128

        if (curr->key != NULL) {
            int size = (int) strlen(curr->key);
            if (size < 1 || size > 128) {
                /*
                request->status_code = request->status_code_list->bad_request;
                request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
                */
                return 1;
            }
        } else {
            /*
            request->status_code = request->status_code_list->bad_request;
            request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
            */
            return 1;
        }

        if (curr->value != NULL) {
            int size = (int) strlen(curr->value);
            if (size > 128) {
                /*
                request->status_code = request->status_code_list->bad_request;
                request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
                */
                return 1;
            }
        } else {
            /*
            request->status_code = request->status_code_list->bad_request;
            request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
            */
            return 1;
        }

        //now check for Content-Length
        if (content_length_found == true && strcmp(curr->key, "Content-Length") == 0) {
            //content-length is repeated in header field
            /*
            request->status_code = request->status_code_list->bad_request;
            request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
            */
            return 1;
        }
        if (strcmp(curr->key, "Content-Length") == 0) { //key is equal to "Content-Length"
            //now check to make sure that value is a digit
            //isdigit == 0, when non-numeric character is passed
            //isdigit == 1, when number character is passed
            char *value_ptr = curr->value;
            while (*value_ptr) {
                if (!isdigit(*value_ptr)) {
                    /*
                    request->status_code = request->status_code_list->bad_request;
                    request->status_phrase = BAD_REQUEST_STATUS_PHRASE;
                    */
                    return 1;
                }
                ++value_ptr;
            }

            //it is a digit
            request->content_length = curr->value;
            content_length_found = true;
        }
        curr = curr->next;
    }
    /*
    request->status_code = request->status_code_list->success;
    request->status_phrase = OK_STATUS_PHRASE;
    */
    return 0;
}
