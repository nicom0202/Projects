#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "my_helper_functs.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>

void socket_error() {
    char error_message[] = "Unable to close Socket\n";
    write(2, error_message, sizeof(error_message) - 1);
}

//this function will write to stderr saying "Invalid Command"
void invalid_error() {
    char error_message[] = "Invalid Command\n";
    write(2, error_message, sizeof(error_message) - 1);
}

//this function will write to stderr saying "Invalid Port"
void invalid_port() {
    char error_message[] = "Invalid Port\n";
    write(2, error_message, strlen(error_message));
}

//this function will write to stderr saying "Invalid Port"
void invalid_request() {
    char error_message[] = "Invalid Request\n";
    write(2, error_message, sizeof(error_message) - 1);
}

//this function will write to stderr saying "Operation Failed"
void operation_error() {
    char error_message[] = "Operation Failed\n";
    write(2, error_message, sizeof(error_message) - 1);
}

//modeled this read and write function
int read_write(int fd1, int fd2) {
    char buf[4096]; // in book this size is a common size p.172
    int bytes_read;
    int bytes_written = 0;

    while ((bytes_read = read(fd1, buf, 4096)) > 0) { //how to read from second line??
        bytes_written = 0;
        while (bytes_written < bytes_read) {
            int temp = write(fd2, buf + bytes_written, bytes_read - bytes_written);

            if (temp == -1) {
                return -1;
            }

            bytes_written += temp;
        }
    }
    return bytes_read;
}

int is_dir(char *filename) {
    struct stat path;
    stat(filename, &path);
    return S_ISREG(path.st_mode);
    //if 0 is returned then it's a directory
}

//returns port number or -1 for failure
int check_if_valid_port(char *port_ptr) {
    //now check to make sure that argument is a digit
    //isdigit == 0, when non-numeric character is passed
    //isdigit == 1, when number character is passed
    char *temp = port_ptr;
    while (*port_ptr) {
        if (!isdigit(*port_ptr)) {
            invalid_port();
            return 1;
        }
        ++port_ptr;
    }

    //have port point back to argv[1] & now it's confirmed that it's an integer
    port_ptr = temp;

    //now create a integer named port
    int port = atoi(port_ptr);

    //now check if port is within range of 1-65535
    if (port < 1 || port > 65535) {
        return -1;
    }

    return port;
}

int read_all_of_socket(int fd) {
    char buf[4096]; // in book this size is a common size p.172
    int bytes_read;

    while ((bytes_read = read(fd, buf, 1024)) > 0) {
        //read until end
    }
    return bytes_read;
}
