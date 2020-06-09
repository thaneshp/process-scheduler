#ifndef VIRTUAL_MEM_MRU_H
#define VIRTUAL_MEM_MRU_H

#include "process_defs.h"
#include "virtual_mem_LRU.h"

/* This script of code defines a group of functions to be used in my customised algorithm */
/* Essentially, these functions are used to execute the virtual memory Most Recently Used (MRU) algorithm */

int evict_pages_MRU(Page_type *mem_array, int no_of_pages, int pages_needed_next, int cum_time, int next_process_id, int no_available);
int find_maximum_arrived_time(Page_type *mem_array, int no_of_pages);
int find_maximum_assigned_page(Page_type *mem_array, int no_of_pages);

int evict_pages_MRU(Page_type *mem_array, int no_of_pages, int pages_needed_next, int cum_time, int next_process_id, int no_available) {
    int i=0, max=0, count=0, start_index=0, next_pages_in_mem=0;
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

    //Find newest pages in memory and evict enough to satisfy execution of next process.
    while (count < pages_to_evict) {
        start_index = find_maximum_assigned_page(mem_array, no_of_pages);
        max = find_maximum_arrived_time(mem_array, no_of_pages);
        for(i=start_index; i>0; i--) {
            if ((mem_array[i].time_arrived == max) & (mem_array[i].process_id > -1) & (mem_array[i].process_id != next_process_id) & (count < pages_to_evict)) {
                mem_array[i].evict = 1;
                mem_array[i].process_id = -1;
                mem_array[i].time_arrived = -1;
                mem_array[i].assigned=0;
                count++;
            }
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

//Helper function to find the newest pages in memory.
int find_maximum_arrived_time(Page_type *mem_array, int no_of_pages) {
    int i=0, max=mem_array[0].time_arrived;

    for(i=0; i<no_of_pages; i++) {
        if (mem_array[i].time_arrived > max) max = mem_array[i].time_arrived;
    }
    return max;
}

//Helper function to find the largest page index.
int find_maximum_assigned_page(Page_type *mem_array, int no_of_pages) {
    int i=0, max=mem_array[0].page_index;

    for(i=0; i<no_of_pages; i++) {
        if (mem_array[i].assigned == 1) {
            if (mem_array[i].page_index > max) max = mem_array[i].page_index;
        }
    }
    return max;
}
#endif