#ifndef BUBBLE_SORT_LINKED_LIST_H
#define BUBBLE_SORT_LINKED_LIST_H

#include "process_defs.h"
#include "linked_list_queue.h"

/* The code used in this program was sourced from https://www.geeksforgeeks.org/c-program-bubble-sort-linked-list/ */
/* It has been adapted to suit the requirements of my customised scheduling algorithm. */

void bubbleSort(struct QNode *start); 
void swap(struct QNode *a, struct QNode *b); 

//Function to bubble sort the given linked list
void bubbleSort(struct QNode *start) { 
    int swapped; 
    struct QNode *ptr1; 
    struct QNode *lptr = NULL; 
  
    //Check for empty list
    if (start == NULL) 
        return; 
  
    do { 
        swapped = 0; 
        ptr1 = start; 
  
        while (ptr1->next != lptr) { 
            if (ptr1->process.running_time > ptr1->next->process.running_time) {  
                swap(ptr1, ptr1->next); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
}

//Function to swap data of two process nodes a and b
void swap(struct QNode *a, struct QNode *b) { 
    Process_type temp = a->process; 
    a->process = b->process; 
    b->process = temp; 
} 


#endif
