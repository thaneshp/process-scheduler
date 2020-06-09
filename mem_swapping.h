#ifndef MEM_SWAPPING_H
#define MEM_SWAPPING_H

#include "process_defs.h"

#define PAGE_LOAD_TIME 2

/* A group of functions to be used with swapping-x memory management algorithm */

int assign_addresses(char *mem_addresses, int pages_needed, int start_page);
int calc_load_time(int memory_size);
int calc_mem_usage(int process_mem_size, int memory_size);
void reset_in_mem(Process_type *in_memory, int no_of_processes);
int evict_process(int cum_time, Process_type *curr_process, Process_type *in_memory);
int check_min_arrival_time(Process_type *in_memory, int no_of_processes, int in_mem_count);

//Function used for assigning memory addresses in swapping-x algorithm.
int assign_addresses(char *mem_addresses, int pages_needed, int start_page) {
    int i = 0, last_page = (pages_needed+start_page)-1;
    char str[MAX_INT_SIZE];

    //String together required pages in ascending order.
    for (i=start_page; i < pages_needed+start_page; i++) {
        sprintf(str, "%d", i);
        if (i==last_page) {
            strcat(mem_addresses, str);
        } else {
            strcat(mem_addresses, str);
            strcat(mem_addresses, ",");
        }
    }
    return i;  
}

//Function used to calculate the load time.
int calc_load_time(int memory_size) {
    return (PAGE_LOAD_TIME*memory_size)/PAGE_SIZE;
}

//Function used to calculate the memory usage for a single process.
int calc_mem_usage(int process_mem_size, int memory_size) {
    double temp_mem_usage;
    int mem_usage;
    
    temp_mem_usage = (((double)process_mem_size/(double)memory_size))*100;
    mem_usage = (int)ceil(temp_mem_usage);

    return mem_usage;
}

//Function to reset all memory in swapping-x algorithm.
void reset_in_mem(Process_type *in_memory, int no_of_processes) {
    int i=0;
    for(i=0;i<no_of_processes;i++) {
        in_memory[i].in_mem = 0;
    }
}

//Function used to evict a process from memory when using swapping-x algorithm.
int evict_process(int cum_time, Process_type *curr_process, Process_type *in_memory) {
    int start_page = curr_process->start_page;
    printf("%d, %s, mem-addresses=[%s]\n", cum_time, EVICTED_STATE, curr_process->mem_addresses);
    curr_process->in_mem = 0;
    in_memory[curr_process->index].in_mem = 0;
    free(curr_process->mem_addresses);
    return start_page;
}

//Function to determine the minimum arrival time for all processes in memory when using swapping-x algorithm.
int check_min_arrival_time(Process_type *in_memory, int no_of_processes, int in_mem_count) {
    int i = 0, count = 0;
    Process_type min_arrival_process; 
    Process_type process[in_mem_count];

    for(i=0; i < no_of_processes; i++) {
        if (in_memory[i].in_mem == 1) {
            process[count] = in_memory[i];
            count++;
        }
    }

    min_arrival_process = process[0]; 
    for(i=0; i < count; i++) {
        if (process[i].mem_arrival_time < min_arrival_process.mem_arrival_time) min_arrival_process = process[i];
    }
    return min_arrival_process.index;
}
#endif
