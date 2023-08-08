#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"

//--------------------------------------------------------------------------------
//Queue
//--------------------------------------------------------------------------------
typedef struct {
    void **data_array;
    int first_out;
    int last_out;
    int queue_size;
    int current_num_of_elements;
    pthread_mutex_t lock;
    pthread_cond_t queue_is_not_empty;
    pthread_cond_t queue_is_not_full;
} Queue;

//--------------------------------------------------------------------------------
//Function: queue_new()
//Parameters: int
//Returns: queue_t pointer
//Purpose: This is the constructer of queue, creates a queue with a max size called
//         size
//--------------------------------------------------------------------------------
queue_t *queue_new(int size) {
    //create a queue struct that has a pointer to the array to hold the elements
    Queue *q = (Queue *) malloc(sizeof(Queue));

    //now create a template type array with a size given by function
    q->data_array = (void **) malloc(sizeof(void *) * size); //see citation (1)

    //initialize each data pointer to NULL
    for (int i = 0; i < size; i++) {
        q->data_array[i] = NULL;
    }

    //initialize all attributes of queue
    q->queue_size = size; //set size of queue
    q->current_num_of_elements = 0; //current number of stuff in the queue is zero

    q->first_out = 0; //nothing is in the queue therefore front of queue is pos 0
    q->last_out = 0; //nothing is in the queue therefore back of queue is pos 0

    //initialize mutexes
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->queue_is_not_empty, NULL);
    pthread_cond_init(&q->queue_is_not_full, NULL);

    //need to ask as queue_t, idk why???
    return (queue_t *) q;
}

//--------------------------------------------------------------------------------
//Function: queue_delete()
//Parameters: an array of queue pointers
//Returns: nothing
//Purpose: Destructor, to delete all dynamically allocated memory
//--------------------------------------------------------------------------------
void queue_delete(queue_t **q) {
    //make a temp pointer pointed to queue
    Queue *queue_temp = (Queue *) *q;

    //iterate over all elements and free each one
    for (int i = 0; i < queue_temp->current_num_of_elements; i++) {
        free(queue_temp->data_array[(queue_temp->first_out + i) % queue_temp->queue_size]);
    }

    //now free data_array
    free(queue_temp->data_array);
    queue_temp->data_array = NULL;

    //free all mutexes
    pthread_mutex_destroy(&queue_temp->lock);
    pthread_cond_destroy(&queue_temp->queue_is_not_empty);
    pthread_cond_destroy(&queue_temp->queue_is_not_full);

    //free queue struct
    free(queue_temp);

    //set it to NULL
    queue_temp = NULL;
}

//--------------------------------------------------------------------------------
//Function: queue_push()
//Parameters: queue_t pointer, template pointer
//Returns: bool
//Purpose: Pushes an element into the array and returns true when it's complete
//--------------------------------------------------------------------------------

bool queue_push(queue_t *q, void *elem) {
    //create a temp queue pointer
    Queue *queue_temp = (Queue *) q;

    //(*) ----CITICAL REGION START-----
    pthread_mutex_lock(&queue_temp->lock); //need to lock becaue about to access shared data

    //if queue pointer is equal to NULL return false
    if (queue_temp == NULL) {
        return false;
    }

    //have thread wait if queue is full (need to wait till another thread has popped something)
    while (
        queue_temp->current_num_of_elements
        == queue_temp
               ->queue_size) { //if current num of elements in queue in the size of the queue, enter while loop
        pthread_cond_wait(&queue_temp->queue_is_not_full,
            &queue_temp
                 ->lock); //want this thread to wait because queue is at MAX cap and can't push anymore
    }

    //now have last out point to elem
    queue_temp->data_array[queue_temp->last_out] = elem;

    //now update last out poitner
    queue_temp->last_out = (queue_temp->last_out + 1) % queue_temp->queue_size; //see citation (2)

    //udpate number of elements in queue
    queue_temp->current_num_of_elements++;

    pthread_cond_signal(
        &queue_temp
             ->queue_is_not_empty); //wakes up threads to let them know that a change has occured and check
    pthread_mutex_unlock(&queue_temp->lock); //unlock

    //(*) ----CITICAL REGION END-----
    return true;
}

//--------------------------------------------------------------------------------
//Function: queue_pop()
//Parameters: queue_t pointer, template pointer
//Returns: bool
//Purpose: Pops an element from the array and returns true when it's complete
//--------------------------------------------------------------------------------
bool queue_pop(queue_t *q, void **elem) {
    //create a temp queue pointer
    Queue *queue_temp = (Queue *) q;

    //(*) ----CITICAL REGION START-----
    pthread_mutex_lock(&queue_temp->lock); //need to lock becaue about to access shared data

    //if queue pointer is equal to NULL return false
    if (queue_temp == NULL) {
        return false;
    }
    //have thread wait if queue is empty (need to wait till another thread has pushed something)
    while (queue_temp->current_num_of_elements == 0) {
        pthread_cond_wait(&queue_temp->queue_is_not_empty, &queue_temp->lock);
    }

    //have elem point to element that will be popped from fornt of queue
    *elem = queue_temp->data_array[queue_temp->first_out];

    //now update first out pointer
    queue_temp->first_out = (queue_temp->first_out + 1) % queue_temp->queue_size; //see citation (2)

    //update number of elements in queue
    queue_temp->current_num_of_elements--;

    pthread_cond_signal(
        &queue_temp
             ->queue_is_not_full); //wakes up threads to let them know that a change has occured and check
    pthread_mutex_unlock(&queue_temp->lock); //unlock

    //(*) ----CITICAL REGION END-----
    return true;
}
