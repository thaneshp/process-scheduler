#ifndef PROCESS_DEFS_H
#define PROCESS_DEFS_H

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define RUNNING_STATE "RUNNING"
#define EVICTED_STATE "EVICTED"
#define FINISHED_STATE "FINISHED"

//Constants used for process and memory management.
#define PAGE_SIZE 4
#define FIRST_PROCESS 0
#define START_PAGE 0
#define MAX_INT_SIZE 11 //Used for printing mem_addresses. Denotes maximum length of an integer string.

//Constants used for calculating performance statistics.
#define MAX_COMMAND_SIZE 3
#define THROUGHPUT_SIZE 3
#define OVERHEAD_SIZE 2
#define NUM_INTERVALS 2
#define STARTING_INTERVAL 0
#define ENDING_INTERVAL 1

//A struct definition to store a single process.
typedef struct Processes {
    int time;
    int process_id;
    int memory_size;
    int running_time;
    int queue_check;
    int expected_time;
    int index;
    char *mem_addresses;
    int in_mem;
    int mem_arrival_time;
    int start_page;
    int start_time;
    int evicted;
} Process_type;

//A struct definition to store user commands.
typedef struct Commands {
    char schedule[MAX_COMMAND_SIZE];
    char memory[MAX_COMMAND_SIZE];
    int mem_size;
    int quant_time;
} Command_type;

//A struct definition to store performance statistics.
typedef struct Stats {
    int throughput[THROUGHPUT_SIZE];
    int turnaround_time;
    double overhead[OVERHEAD_SIZE];
    int makespan;
} Stats_type;

//A struct definition to store attributes of a page in memory.
//Used specifically in virtual memory LRU & MRU algorithms.
typedef struct Page {
    int page_index;
    int time_arrived;
    int process_id;
    int assigned;
    int evict;
} Page_type;

#endif