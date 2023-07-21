#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include "asgn2_helper_funcs.h"
#include "my_helper_functs.h"
#include "parse.h"
#include "response.h"

#define FORBIDDEN "Forbidden"
//--------------------------------------------------------------------------------------------------------------------
//(*) NOTE: NEED TO DYNAMICALLY ALLOCATE A REQUEST STRUCT POINTER, THEN WHENEVER YOU ASSIGN ONE OF IT'S ATTRIBUTES
//          TO SOMETHING YOU NEED TO DYNAMICALLY ALLOCATE IT'S ATTRIBUTES THEN HAVE IT POINT TO THEM
//--------------------------------------------------------------------------------------------------------------------

void free_all_memory(Request *, Response *, Status_Code *);

int main(int argc, char **argv) {

    //only want two arguements when running this program: ./httperserver <port>
    if (argc != 2) {
        invalid_port();
        return 1;
    }

    //check if port number is valid
    char *port_ptr = argv[1];
    int port = check_if_valid_port(port_ptr);
    if (port == -1) {
        invalid_port();
        return 1;
    }

    //now use helper fucntions to bind the socket to the port
    Listener_Socket sock;
    if (listener_init(&sock, port) == 1) {
        //failed (either because port was in an invalid range, or because it could not bind to port)
        invalid_port();
        return 1;
    }

    //-----------NOTE: THIS SHOULD BE PUT IN A WHILE LOOP TO LISTEN FOREVER----------
    while (1) {
        int fd_socket = listener_accept(
            &sock); //INITIALIZES A 5-SECOND TIMEOUT --> FIGURE OUT WHAT THIS MEANS

        int request_buf_max_size = 2048;
        //char req_buf[request_buf_max_size];
        char *req_buf = calloc(request_buf_max_size,
            sizeof(
                char)); //this makes a dynamic array of size 2048 and each element is size of char and initalized to 0
        char *delim = "\r\n\r\n";
        int buf_bytes_read = 0;

        //(*) THIS WILL READ 2048!!!!! but it will return the position of the delimiter
        buf_bytes_read = read_until(fd_socket, req_buf, request_buf_max_size, delim);
        //check for error
        if (buf_bytes_read == -1) {
            //MAKE A REPONSE OF AN INTERNNAL ERROR MESSAGE
            internal_server_error(fd_socket);
            close(fd_socket);
            continue;
        }
        if (buf_bytes_read == 0) {
            empty_response(fd_socket);
            close(fd_socket);
            continue;
        }
        /*
        if(strlen(req_buf) == 0){
            empty_response(fd_socket);
            close(fd_socket);
            continue;
        }
        */
        //creating a status code repsoittory for Request to have quick access to
        Status_Code *status_code_repository = malloc(sizeof(Status_Code));
        status_code_repository->success = "200\0";
        status_code_repository->uri_created = "201\0";
        status_code_repository->bad_request = "400\0";
        status_code_repository->forbidden = "403\0";
        status_code_repository->not_found = "404\0";
        status_code_repository->internal_server_error = "500\0";
        status_code_repository->not_implemented = "501\0";
        status_code_repository->version_not_supported = "505\0";

        //now send to parsing module to deal with whether this is a valid or invalid request
        //initializing request
        Request *request = malloc(sizeof(Request));
        request->method = NULL;
        request->buffer = req_buf;
        request->uri = NULL;
        request->version = NULL;
        request->buf_size = 2048;
        request->fd_socket = fd_socket;
        request->end_of_header_field_pos = -1;
        request->content_length = NULL; //for get, if content length is != NULL then its wrong
        request->status_code = NULL;
        request->status_phrase = NULL;
        request->status_code_list = status_code_repository;
        request->header_field = NULL;

        //(*)NOTE: This checks ONLY IF the request is valid for parsing, DOES NOT CHECK IF FILE EXISTS OR IF CONTENTS SHOULD BE THERE OR NOT FOR GET/PUT REQUESTS
        int result = parse_handler(req_buf, request);

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            //make response for client not sending anything
            //read_all_of_socket(fd_socket);
            //set_opened_request(fd_socket);
            empty_response(fd_socket);
            if (strcmp(request->method, "PUT") == 0) {
                int fd = open(request->uri, O_CREAT | O_WRONLY | O_TRUNC, 0666);

                int success = write_all(
                    fd, request->buffer, buf_bytes_read - request->end_of_header_field_pos);
                if (success < 0) {
                    //internal server error
                }
                //step 2: write the contents thats left in the socket
                success = read_write(fd_socket, fd);
                if (success < 0) {
                    //internal server error
                }

                //check to make sure file size matches content-length
                //if it doesn't then delete file, close it and send an invalid repsonse
            }
            close(fd_socket);
            continue;
        }

        if (result != 0) {

            //read the rest of the socket
            if (read_all_of_socket(fd_socket)
                == -1) { //need to read everything even tho it's invalid
                //internal server error
            }

            //write error message back to client
            if (result == 1) {
                bad_request(fd_socket);
            }
            if (result == 2) {
                internal_server_error(fd_socket);
            }
            if (result == 3) {
                not_implemented_request(fd_socket);
            }
            if (result == 4) {
                version_not_supported_request(fd_socket);
            }

            //free memory
            free_all_memory(request, NULL, status_code_repository);

            //close socket
            close(fd_socket);

            //continue to next while loop iteration
            continue;
        }

        //(********************************** GET **********************************)

        if (strcmp(request->method, "GET") == 0) { //method is GET

            //preliminary checks
            //(1) no header field
            //(2) no content-length
            //(3) make sure bytes_read == request->end_of_header_field_pos
            if (request->content_length != NULL || request->content_length != NULL
                || buf_bytes_read != request->end_of_header_field_pos) {

                //read the rest of the socket
                if (read_all_of_socket(fd_socket)
                    == -1) { //need to read everything even tho it's invalid
                    //internal server error
                }
                bad_request(
                    fd_socket); //return invalid request (these are bad requests) -->hard code the bad requests and send it to client
                free_all_memory(request, NULL, status_code_repository); //free memory
                close(fd_socket); //close socket
                continue;
            }

            //(4) check if theres extra stuff in the socket
            //      (*)NOTE: the read_until will read at most 2048 bytes (max size for request), now read again for one byte and if theres is something throw an invalid request
            char extra_stuff[3];
            if (read_until(fd_socket, extra_stuff, 3, NULL) > 0) {

                //read the rest of the socket
                if (read_all_of_socket(fd_socket)
                    == -1) { //need to read everything even tho it's invalid
                    //internal server error
                }
                bad_request(
                    fd_socket); //return invalid request (these are bad requests) -->hard code the bad requests and send it to client
                free_all_memory(request, NULL, status_code_repository); //free memory
                close(fd_socket); //close socket
                continue;
            }

            //-----new-----

            // if(errno == EAGAIN || errno == EWOULDBLOCK){
            //     //make response for client not sending anything
            //     read_all_of_socket(fd_socket);
            //     set_opened_request(fd_socket);
            //     close(fd_socket);
            //     continue;
            // }

            //(******************************* FILE HANDLING *******************************)

            int fd = open(request->uri, O_RDONLY, 0); //read only and permissions=0
            // struct stat sta;
            struct stat st;

            //printf("----%s-----\n",request->uri);

            if ((access(request->uri, F_OK) != 0)) {
                get_not_found_request(fd_socket);
                free_all_memory(request, NULL, status_code_repository); //free memory
                close(fd_socket); //close socket
                continue;
            }
            //struct stat st;

            if (stat(request->uri, &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    forbidden_request(fd_socket);
                    free_all_memory(request, NULL, status_code_repository); //free memory
                    close(fd_socket); //close socket
                    continue;
                }
            }

            // if (fd == -1) {
            //     get_not_found_request(fd_socket);
            //     free_all_memory(request,NULL,status_code_repository);//free memory
            //     close(fd_socket);//close socket
            //     continue;
            // }

            //(****** file size ******)
            //get file size
            if (stat(request->uri, &st) != 0) {
                //write internal server error here
            }
            int file_size = (int) st.st_size;
            //filesize
            //(*) NEED A WAY TO MAKE AN INT INTO A STRING!!!!!!
            char file_char[20] = { 0 };
            sprintf(file_char, "%i", file_size);

            //make valid response to client
            valid_get_response(fd_socket, file_char);

            /*
            //write contents in file to client
            if(read_write(fd, fd_socket) == -1){
                //catch error
            }
            */
            int result = pass_bytes(fd, fd_socket, file_size);
            if (result == -1) {
                //error reading
            }
            if (result == -2) {
                //error writing
            }

            free_all_memory(request, NULL, status_code_repository); //free memory
            close(fd_socket);
            continue;
        }

        //(********************************* SET *********************************************)
        if (strcmp(request->method, "PUT") == 0) {

            //check to make sure content-length exists
            if (request->content_length == NULL) {
                bad_request(fd_socket);
                free_all_memory(request, NULL, status_code_repository);
                close(fd_socket);
                continue;
            }

            //check if file exists, is writeable and the size is the same as content_length
            if (access(request->uri, F_OK) == 0
                && access(request->uri, W_OK) != 0) { //if file exists and is not writeable
                forbidden_request(fd_socket);
                free_all_memory(request, NULL, status_code_repository);
                close(fd_socket);
            }

            int created = false;
            int overwritten = false;
            if (access(request->uri, F_OK) == 0
                && access(request->uri, W_OK) == 0) { //file exists and is writeable
                overwritten = true;
            } else {
                created = true;
            }

            int fd = open(request->uri, O_CREAT | O_WRONLY | O_TRUNC,
                0666); //read only and permissions maybe need O_RDONLY
            if (fd < 0) {
                //internal error
            }

            //(********************* WRITING TO FILE **************************)
            //step 1: write the contents that's in the buffer
            int success
                = write_all(fd, request->buffer, buf_bytes_read - request->end_of_header_field_pos);
            if (success < 0) {
                //internal server error
            }

            //step 2: write the contents thats left in the socket
            success = read_write(fd_socket, fd);
            if (success < 0) {
                //internal server error
            }

            //check to make sure file size matches content-length
            //if it doesn't then delete file, close it and send an invalid repsonse

            //now make a response
            if (created) {
                valid_put_created_response(fd_socket);
            } else if (overwritten) {
                valid_put_overwritten_response(fd_socket);
            }

            free_all_memory(request, NULL, status_code_repository); //free memory

            //close socket
            close(fd_socket);
            continue;
        }

        signal(SIGPIPE,
            SIG_IGN); //(*) make a fucntion that handles this signal code --> want to free memory and other stuff when hitting ^C in terminal
    }
    return 0;
}

void free_all_memory(Request *request, Response *response, Status_Code *status_codes) {

    //free status_codes
    free(status_codes);
    status_codes = NULL;

    //free request
    while (request->header_field != NULL) {
        struct Header_Field *temp = request->header_field;
        request->header_field = request->header_field->next;
        temp->next = NULL;
        if (temp->key != NULL)
            free(temp->key);
        if (temp->value != NULL)
            free(temp->value);
        free(temp);
        temp = NULL;
    }

    if (request->method != NULL)
        free(request->method);
    /*
    if(request->buffer != NULL) //undefined behavior for free static variable 
        free(request->buffer);
    */

    if (request->uri != NULL)
        free(request->uri);

    if (request->version != NULL)
        free(request->version);

    //free(request->buf_size);  //can't free ints
    //free(request->fd_socket); //can't free ints
    //free(request->end_of_header_field_pos); //can't free ints
    //free(request->content_length); //don't free content_length because this just points to a header_field->value and it will get freed ^^^^

    if (request->status_code != NULL)
        free(request->status_code);

    if (request->status_phrase != NULL)
        free(request->status_phrase);
    //free(request->request_size); //can't free ints

    /* ALREADY FREED STATUS CODES AND MADE THEM POINT TO NULL
    if(request->status_code_list != NULL)
        free(request->status_code_list);
    */

    if (request != NULL)
        free(request);
    request = NULL;

    //free repsonse
    if (response != NULL)
        free(response);
    response = NULL;
}
