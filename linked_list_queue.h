#ifndef LINKED_LIST_QUEUE_H
#define LINKED_LIST_QUEUE_H

#include "process_defs.h"

/* The code for this program was sourced from https://www.geeksforgeeks.org/queue-linked-list-implementation/ */
/* It has been adapted to suit the needs of this process scheduling simulator */

struct QNode* newNode(Process_type p);
struct Queue* createQueue();
void enQueue(struct Queue* q, Process_type p);
void deQueue(struct Queue* q);
int getCount(struct QNode* head);
Process_type *search(struct QNode* head, int process_id);

// A linked list (LL) node to store a queue entry 
struct QNode { 
    Process_type process; 
    struct QNode* next; 
}; 
  
// The queue, front stores the front node of LL and rear stores the 
// last node of LL 
struct Queue { 
    struct QNode *front, *rear; 
}; 

// A utility function to create a new linked list node. 
struct QNode* newNode(Process_type p) { 
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode)); 
    temp->process = p; 
    temp->next = NULL; 
    return temp; 
} 

// A utility function to create an empty queue 
struct Queue* createQueue() { 
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue)); 
    q->front = q->rear = NULL; 
    return q; 
} 

// Function to add a process p to q 
void enQueue(struct Queue* q, Process_type p) { 
    // Create a new LL node 
    struct QNode* temp = newNode(p); 
  
    // If queue is empty, then new node is front and rear both 
    if (q->rear == NULL) { 
        q->front = q->rear = temp; 
        return; 
    } 
  
    // Add the new node at the end of queue and change rear 
    q->rear->next = temp; 
    q->rear = temp; 
} 

// Function to remove a process from given queue q 
void deQueue(struct Queue* q) { 
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
        return; 
  
    // Store previous front and move front one node ahead 
    struct QNode* temp = q->front; 
  
    q->front = q->front->next; 
    
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
        q->rear = NULL; 

    free(temp); 
}

//Function to count number of processses in queue
int getCount(struct QNode* head) { 
    int count = 0;  // Initialize count 
    struct QNode* current = head;  // Initialize current 

    while (current != NULL) { 
        count++; 
        current = current->next; 
    } 
    return count; 
}

// Function to search for a process given it's process id
Process_type *search(struct QNode* head, int process_id) { 
    struct QNode *current = head;  // Initialize current 

    //Traverse queue until process is found
    while (current != NULL) { 
        if (current->process.process_id == process_id) {
            return &current->process; 
        }
        current = current->next; 
    }
    return &current->process; 
} 
#endif