#include "asgn4_helper_funcs.h"
#include "connection.h"
#include "debug.h"
#include "request.h"
#include "response.h"
#include "queue.h"

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>

#include <sys/stat.h>
#include <sys/file.h>

#define DEFAULT_THREADS 4
#define RID             "Request-Id"

// Gloabls
queue_t *workq = NULL;
const char *LOCK_FILE = "lockfile.lock";

// Prototypes
void handle_connection(void);
void handle_get(conn_t *);
void handle_put(conn_t *);
void handle_unsupported(conn_t *);
char *get_rid(conn_t *);
void audit_log(char *, char *, char *, int);
void useage(FILE *, char *);
void acquire_exclusive(int);
void acquire_shared(int);
int acquire_templock(void);
void release(int);

int main(int argc, char **argv) {
    int opt = 0;
    int threads = DEFAULT_THREADS;
    pthread_t *threadids;

    if (argc < 2) {
        warnx("wrong arguments: %s port_num", argv[0]);
        useage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    while ((opt = getopt(argc, argv, "t:h")) != -1) {
        switch (opt) {
        case 't':
            threads = strtol(optarg, NULL, 10);
            if (threads <= 0) {
                errx(EXIT_FAILURE, "bad number of threads");
            }
            break;

        case 'h': useage(stdout, argv[0]); return EXIT_SUCCESS;

        default: useage(stderr, argv[0]); return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        warnx("wrong arguments: %s port_num", argv[0]);
        useage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    //~~~~~~~~~~~~~~~~
    //port number
    //~~~~~~~~~~~~~~~~
    char *endptr = NULL;
    size_t port = (size_t) strtoull(argv[optind], &endptr, 10);

    if (endptr && *endptr != '\0') {
        warnx("invalid port number: %s", argv[1]);
        return EXIT_FAILURE;
    }

    signal(SIGPIPE, SIG_IGN);
    Listener_Socket sock;
    if (listener_init(&sock, port) < 0) {
        warnx("Cannot open listener sock: %s", argv[0]);
        return EXIT_FAILURE;
    }

    threadids = malloc(sizeof(pthread_t) * threads);
    workq = queue_new(threads);
    creat(LOCK_FILE, 0600);

    for (int i = 0; i < threads; i++) {
        int rc = pthread_create(threadids + i, NULL, (void *(*) (void *) ) handle_connection, NULL);
        if (rc != 0) {
            warnx("Cannot create %d pthreads", threads);
            return EXIT_FAILURE;
        }
    }

    while (1) {
        uintptr_t connfd = listener_accept(&sock);
        //debug("accepted %lu\n", connfd);
        queue_push(workq, (void *) connfd);
    }

    queue_delete(&workq);

    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: handle_connection()
//Paratmeter: void
//Return: void
//Purpose: The queue will push this function and handle_connection will call GET or PUT
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void handle_connection(void) {
    while (true) {
        uintptr_t connfd = 0;
        conn_t *conn = NULL;

        queue_pop(workq, (void **) &connfd);

        //debug("popped off %lu", connfd);
        conn = conn_new(connfd);

        const Response_t *res = conn_parse(conn); //if returns NULL, then it is correct

        if (res != NULL) {
            conn_send_response(conn, res); //bad request
        } else {
            //debug("%s", conn_str(conn));
            const Request_t *req = conn_get_request(conn);

            if (req == &REQUEST_GET) {
                handle_get(conn);
            } else if (req == &REQUEST_PUT) {
                handle_put(conn);
            } else {
                handle_unsupported(conn);
            }
        }
        conn_delete(&conn);
        close(connfd);
    }
    return;
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: handle_get()
//Paratmeter: conn_t*
//Return: void
//Purpose: Handles GET logic
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void handle_get(conn_t *conn) {
    //(1) Lock temp file
    int lock = acquire_templock();

    //(0) Declare Variables
    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;

    //(2) Open uri
    int fd = open(uri, O_RDONLY);

    if (fd < 0) {
        struct stat file_stat;
        if (stat(uri, &file_stat) == 0 || S_ISDIR(file_stat.st_mode)) {
            res = &RESPONSE_FORBIDDEN; // File is a directory or restrictied
        } else {
            res = &RESPONSE_NOT_FOUND; // File does not exist- status_code: 404 NOT FOUND
        }
        goto out;
    }

    //(3) Acquire a shared lock for GET
    acquire_shared(fd);

    //(4) Release & close temp lock
    release(lock);
    close(lock);

    //(5) Get the file size
    off_t file_size = 0;
    struct stat st;
    if (stat(uri, &st) == 0) {
        file_size = st.st_size; // File size in bytes
    } else {
        res = &RESPONSE_INTERNAL_SERVER_ERROR;
        goto out;
    }

    //(6) Send response and file to client
    conn_send_file(conn, fd, file_size);

out:
    //(7) Send a resposne if res is not NULL (aka something went wrong) & audit log
    if (res != NULL) {
        conn_send_response(conn, res);
        audit_log("GET", uri, get_rid(conn), response_get_code(res));
    } else {
        audit_log("GET", uri, get_rid(conn), response_get_code(&RESPONSE_OK));
    }

    if (fd > 0) {
        release(fd);
        close(fd);
    } else {
        release(lock);
        close(lock);
    }
    return;
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: handle_put()
//Paratmeter: conn_t*
//Return: void
//Purpose: Handles PUT logic
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void handle_put(conn_t *conn) {
    //(1) Lock temp file
    int lock = acquire_templock();

    //(0) Delcare varibales
    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;

    //(2) Check if file exists or not
    bool exists = access(uri, F_OK) == 0; //True if exists, False if it does NOT exist

    //(3) Open the file
    int fd = open(uri, O_CREAT | O_WRONLY, 0600);

    //(5) directory check goto out
    struct stat file_stat;
    if (stat(uri, &file_stat) == 0 && S_ISDIR(file_stat.st_mode)) {
        res = &RESPONSE_FORBIDDEN;
        goto out;
    }

    //(4) Check if server error or forbidden
    if (fd < 0) {
        if (errno == EACCES) {
            res = &RESPONSE_FORBIDDEN;
        } else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
        }
        goto out;
    }

    //(6) Acquire exclusive lock for SET
    acquire_exclusive(fd);

    //(7) Release & close temp lock
    release(lock);
    close(lock);

    //(8) Truncate file
    int rc = ftruncate(fd, 0);
    assert(rc == 0);

    //(9) Receive the file
    conn_recv_file(conn, fd); // write the data from the connection into the file (fd).

    //(10) if res is NULL and the file exists then respond with OK
    if (res == NULL && exists == true) {
        conn_send_response(conn, &RESPONSE_OK);
        audit_log("PUT", uri, get_rid(conn), response_get_code(&RESPONSE_OK));
    }
    //(11) if res is NULL and the file does not exist then respond with CREATED
    if (res == NULL && exists == false) {
        conn_send_response(conn, &RESPONSE_CREATED);
        audit_log("PUT", uri, get_rid(conn), response_get_code(&RESPONSE_CREATED));
    }
out:

    //(12) Send a response if res is not NULL & audit log
    if (res != NULL) {
        conn_send_response(conn, res);
        audit_log("PUT", uri, get_rid(conn), response_get_code(res));
    }

    if (fd > 0) {
        release(fd);
        close(fd);
    } else {
        release(lock);
        close(lock);
    }

    return;
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: handle_unsupported()
//Paratmeter: conn_t*
//Return: void
//Purpose: Sends a response to client that the request has an unsupported version
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void handle_unsupported(conn_t *conn) {
    //debug("Unsupported request");
    conn_send_response(conn, &RESPONSE_NOT_IMPLEMENTED);
    return;
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: get_rid()
//Paratmeter: conn_t*
//Return: char*
//Purpose: Gets the request-id from conn_t* and if there is no id, id is set to 0
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
char *get_rid(conn_t *conn) {
    char *id = conn_get_header(conn, RID);
    if (id == NULL) {
        id = "0";
    }
    return id;
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: audit_log()
//Paratmeter: char*, char*, char*, int
//Return: void
//Purpose: Keeps an audit log of requests, sends it to stderr
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void audit_log(char *operation, char *uri, char *id, int code) {
    fprintf(stderr, "%s,/%s,%d,%s\n", operation, uri, code, id);
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: useage()
//Paratmeter: FILE*, char*
//Return: void
//Purpose: To tell user how to use httpserver.c
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void useage(FILE *stream, char *exec) {
    fprintf(stream, "usage: %s [-t threads] <port>\n", exec);
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: acquire_exclusive()
//Paratmeter: int
//Return: void
//Purpose: Acquires an exclusive flock lock on file descriptor
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void acquire_exclusive(int fd) {
    int rc = flock(fd, LOCK_EX);
    assert(rc == 0);
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: acquire_shared()
//Paratmeter: int
//Return: void
//Purpose: Acquires a shared flock lock on file descriptor
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void acquire_shared(int fd) {
    int rc = flock(fd, LOCK_SH);
    assert(rc == 0);
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: acquire_templock()
//Paratmeter: void
//Return: int
//Purpose: Acquires an exclusive flock lock on a temporary file descriptor
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
int acquire_templock(void) {
    int fd = open(LOCK_FILE, O_WRONLY);
    acquire_exclusive(fd);
    return fd;
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//Function: release()
//Paratmeter: int
//Return: void
//Purpose: Releases any flock lock on a file descriptor
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void release(int fd) {
    flock(fd, LOCK_UN);
}
