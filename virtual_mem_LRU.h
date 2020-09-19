#ifndef VIRTUAL_MEM_LRU_H
#define VIRTUAL_MEM_LRU_H

#include "process_defs.h"

/* This script of code defines a group of functions used with the virtual memory algorithm */
/* Most functions here are used in both LRU and MRU but some are LRU specific */

int evict_pages_finished(Page_type *mem_array, int no_of_pages, Process_type *curr_process, int cum_time, Process_type *next_process);
int evict_pages_LRU(Page_type *mem_array, int no_of_pages, int pages_needed_next, int process_start_time, int cum_time, int next_process_id, int no_available);
void generate_mem_addresses(Page_type *mem_array, int no_of_pages, Process_type *process, int pages_needed, char *mem_addresses);
void generate_evicted_mem_addresses(Page_type *mem_array, int no_of_pages, int pages_needed_next, char *mem_address);
void assign_pages(Page_type *mem_array, int needed_pages, int process_id, int cum_time, int no_of_pages);
int calc_process_in_memory(int process_id, int no_of_pages, Page_type *mem_array);
void initialse_memory(Page_type *mem_array, int no_of_pages);
int calc_pages_in_mem(Page_type *mem_array, int no_of_pages);
int find_minimum_arrived_time(Page_type *mem_array, int no_of_pages);

//Evicting pages for a finished process.
int evict_pages_finished(Page_type *mem_array, int no_of_pages, Process_type *curr_process, int cum_time, Process_type *next_process) {
    int i=0, next_pages_in_mem =0, count = 0, pages_needed_next = 0;

    for(i=0; i<no_of_pages; i++) {
        if (mem_array[i].process_id == curr_process->process_id) {
            mem_array[i].evict = 0;
            mem_array[i].process_id = -1;
            mem_array[i].time_arrived = -1;
            mem_array[i].assigned=0;
            count++;
        }
    }

    printf("%d, %s, mem-addresses=[%s]\n", cum_time, EVICTED_STATE, curr_process->mem_addresses);

    //Find out the amount of pages required for next process once current process has finished.
    if (next_process != NULL) {
        next_pages_in_mem = calc_process_in_memory(next_process->process_id, no_of_pages, mem_array);
        pages_needed_next = next_process->memory_size/PAGE_SIZE;
        pages_needed_next = pages_needed_next - next_pages_in_mem;
        if (count < pages_needed_next) pages_needed_next = count;
    }

    return pages_needed_next;
}

//Evict pages necessary for next process when using LRU algorithm
int evict_pages_LRU(Page_type *mem_array, int no_of_pages, int pages_needed_next, int process_start_time, int cum_time, int next_process_id, int no_available) {
    int i=0, count=0, next_pages_in_mem=0, min=0;
    char *mem_address;
    
    //Check whether next process already has pages in memory.
    next_pages_in_mem = calc_process_in_memory(next_process_id, no_of_pages, mem_array);
    pages_needed_next = pages_needed_next - next_pages_in_mem;
    //Allocate necessary amount of pages needed for next process.
    if ((next_pages_in_mem + pages_needed_next > 4) & (no_available == 0)) {
        while (next_pages_in_mem + pages_needed_next > 4) pages_needed_next = pages_needed_next - 1;
    } else if ((no_available > 0) & (pages_needed_next > 4)) {
        pages_needed_next = 4;
    }

    int pages_to_evict = pages_needed_next - no_available;
    
    //Find the oldest page in memory and evict these first.
    min = find_minimum_arrived_time(mem_array, no_of_pages);
    for(i=0; i<no_of_pages; i++) {
        if ((mem_array[i].time_arrived == min) & (mem_array[i].process_id > -1) & (mem_array[i].process_id != next_process_id) & (count < pages_to_evict)) {
            mem_array[i].evict = 1;
            mem_array[i].process_id = -1;
            mem_array[i].time_arrived = -1;
            count++;
        }
    }
    
    //Evict pages where they have arrived prior to the current processes start time.
    for(i=0; i<no_of_pages; i++) {
        if ((mem_array[i].time_arrived < process_start_time) & (mem_array[i].process_id > -1) & (mem_array[i].process_id != next_process_id) & (count < pages_to_evict)) {
            mem_array[i].evict = 1;
            mem_array[i].process_id = -1;
            mem_array[i].time_arrived = -1;
            count++;
        }
    }
    
    //Evict pages beginning from the start if there are still more pages required.
    if (count < pages_to_evict) {
        for(i=0; i<(pages_to_evict-count); i++) {
            mem_array[i].evict = 1;
            mem_array[i].process_id = -1;
            mem_array[i].time_arrived = -1;
        }
    }

    //Print the evicted pages.
    if (pages_to_evict > 0) {
        mem_address = malloc((pages_to_evict * sizeof(char)) + (pages_to_evict - 1));
        memset(mem_address, 0, (pages_to_evict * sizeof(char)) + (pages_to_evict - 1));
        generate_evicted_mem_addresses(mem_array, no_of_pages, pages_to_evict, mem_address);
        printf("%d, %s, mem-addresses=[%s]\n", cum_time, EVICTED_STATE, mem_address);
        free(mem_address);
    }

    return pages_needed_next;
}

//Create a string of memory addresses used for printing purposes when memory is being assigned.
void generate_mem_addresses(Page_type *mem_array, int no_of_pages, Process_type *process, int pages_needed, char *mem_addresses) {
    int i=0, count=0;
    char str[MAX_INT_SIZE];

    //Cycle through memory to join necessary pages in a string like format.
    for(i=0; i < no_of_pages; i++) {
        if (mem_array[i].process_id == process->process_id) {
            sprintf(str, "%d", mem_array[i].page_index);
            if (count == (pages_needed - 1)) {
                strcat(mem_addresses, str);
            } else {
                strcat(mem_addresses, str);
                strcat(mem_addresses, ",");
                count++;
            }
        } 
    }
}

//Create a string of memory addresses used for printing purposed when memory is being evicted.
void generate_evicted_mem_addresses(Page_type *mem_array, int no_of_pages, int pages_needed_next, char *mem_address) {
    int i=0, count = 0;
    char str[MAX_INT_SIZE];

    //Cycle through memory to join necessary pages in a string like format.
    for (i=0; i<no_of_pages; i++) {
        if (mem_array[i].evict == 1) {
            sprintf(str, "%d", mem_array[i].page_index);
            if (count == (pages_needed_next - 1)) {
                strcat(mem_address, str);
                mem_array[i].time_arrived = -1;
                mem_array[i].evict = 0;
            } else {
                strcat(mem_address, str);
                strcat(mem_address, ",");
                mem_array[i].time_arrived = -1;
                mem_array[i].evict = 0;
                count++;
            }
        }
    }
}

//Assign the necessary amount of pages for a process given the amount it requires for it's execution.
void assign_pages(Page_type *mem_array, int needed_pages, int process_id, int cum_time, int no_of_pages) {
    int i = 0, count=0;

    count = calc_process_in_memory(process_id, no_of_pages, mem_array);

    for (i=0; i< no_of_pages; i++) {
        if ((mem_array[i].process_id == -1) & (count != needed_pages)) {
            mem_array[i].process_id = process_id;
            mem_array[i].time_arrived = cum_time;
            mem_array[i].assigned = 1;
            count++;
        }
    }
}

//Function to determine how many pages a given process has in memory.
int calc_process_in_memory(int process_id, int no_of_pages, Page_type *mem_array) {
    int i=0, count=0;

    for(i=0; i<no_of_pages; i++) {
        if (mem_array[i].process_id == process_id) {
            count++;
        }
    }
    return count;
}

//Function used to reset all values in memory.
void initialse_memory(Page_type *mem_array, int no_of_pages) {
    int i = 0;

    for (i=0; i< no_of_pages; i++) {
        mem_array[i].page_index = i;
        mem_array[i].process_id = -1;
        mem_array[i].time_arrived = -1;
        mem_array[i].evict=0;
    }
}

//Function to calculate the amount of pages currently being used.
int calc_pages_in_mem(Page_type *mem_array, int no_of_pages) {
    int i =0, count=0;

    for (i=0; i < no_of_pages; i++) {
        if (mem_array[i].process_id != -1) {
            count++;
        }
    }
    return count;
}

//Helper function to find the oldest time a page was assigned in memory.
int find_minimum_arrived_time(Page_type *mem_array, int no_of_pages) {
    int i=0, min=mem_array[0].time_arrived;
    for(i=0; i<no_of_pages; i++) {
        if (mem_array[i].time_arrived < min) min = mem_array[i].time_arrived;
    }
    return min;
}
#endif