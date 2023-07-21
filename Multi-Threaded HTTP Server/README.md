-----------------------------------------------------------------------------------------------------------------------
Functions
-----------------------------------------------------------------------------------------------------------------------
All descriptions of functions are in httpserver.c where it is implemented

    (1) void handle_connection(void);
    (2) void handle_get(conn_t *);
    (3) void handle_put(conn_t *);
    (4) void handle_unsupported(conn_t *);
    (5) char* get_rid(conn_t *);
    (6) void audit_log(char *, char *, char *, int);
    (7) void useage(FILE *, char *);
    (8) void acquire_exclusive(int);
    (9) void acquire_shared(int);
    (10) int acquire_templock(void);
    (11) void release(int);

-----------------------------------------------------------------------------------------------------------------------
Purpose
-----------------------------------------------------------------------------------------------------------------------
To create a thread safe HTTP server 

-----------------------------------------------------------------------------------------------------------------------
Citations
-----------------------------------------------------------------------------------------------------------------------
(1) Starter code from resources

(2) All functions in the function catagory are build off of Mich's discussion section