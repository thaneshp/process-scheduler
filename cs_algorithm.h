#ifndef CS_ALGORITHM_H
#define CS_ALGORITHM_H

#include "process_defs.h"
#include "bubble_sort_linked_list.h"

/* This program is an implementation of Non-Preemptive Shortest Job First Algorithm. */

void cs_algorithm(int no_of_processes, Process_type *processes, char *memory, int memory_size);

//Function to execute Non-Preemptive Shortest Job First Algorithm given an array of processes and memory requirements.
void cs_algorithm(int no_of_processes, Process_type *processes, char *memory, int memory_size) {
    int cum_time = processes[FIRST_PROCESS].time, num_proc_left = 0, total_finished = 0;

    //Variables used for virtual memory LRU algorithm.
    int load_time=0, mem_usage=0, pages_needed=0;
    int no_of_pages = memory_size/PAGE_SIZE;
    //Initialise memory array used in LRU.
    Page_type mem_array[no_of_pages];
    initialse_memory(mem_array, no_of_pages);

    //Variables needed for statistic calculations.
    double curr_overhead=0.0;
    int count=0;
    int turnaround_times[no_of_processes], time_stamps[no_of_processes];
    Stats_type cs_stats;

    //Create queue and insert first process.
    struct Queue* q = createQueue();
    enQueue(q, processes[FIRST_PROCESS]);
    processes[FIRST_PROCESS].queue_check = 1;

    while (!((q->front == NULL) && (q->rear == NULL))) {
        Process_type curr_process = q->front->process;
        if (strcmp(memory, "u")==0) printf("%d, %s, id=%d, remaining-time=%d\n", cum_time, RUNNING_STATE, curr_process.process_id, curr_process.running_time);
        else if (strcmp(memory, "v")==0) printf("%d, %s, id=%d, remaining-time=%d", cum_time, RUNNING_STATE, curr_process.process_id, curr_process.running_time);
        cum_time = cum_time + curr_process.running_time;

        //Run virtual memory LRU algorithm.
        if (strcmp(memory,"v")==0) {
            //Determine amount of memory needed for current process.
            load_time = calc_load_time(curr_process.memory_size);
            mem_usage = calc_mem_usage(curr_process.memory_size, memory_size);
            pages_needed = curr_process.memory_size/PAGE_SIZE;

            //Assign necessary amount of pages this process requires.
            assign_pages(mem_array, pages_needed, curr_process.process_id, cum_time, no_of_pages);
            curr_process.mem_addresses = malloc((pages_needed * sizeof(char)) + (pages_needed - 1));
            memset(curr_process.mem_addresses, 0, (pages_needed * sizeof(char)) + (pages_needed - 1));
            generate_mem_addresses(mem_array, no_of_pages, &curr_process, pages_needed, curr_process.mem_addresses);
            printf(", load-time=%d, mem-usage=%d%%, mem-addresses=[%s]\n", load_time, mem_usage, curr_process.mem_addresses);

            //Evict memory once process is complete.
            cum_time = cum_time + load_time;
            initialse_memory(mem_array, no_of_pages);
            printf("%d, %s, mem-addresses=[%s]\n", cum_time, EVICTED_STATE, curr_process.mem_addresses);
            free(curr_process.mem_addresses);  
        }

        //Add processes that have arrived in the meantime to the end of the queue.
        int i = 0, processes_arrived = 0;
        for (i=0; i < no_of_processes; i++) {
            if ((processes[i].queue_check != 1) && (processes[i].time <= cum_time)) {
                processes[i].queue_check = 1;
                enQueue(q, processes[i]);
                processes_arrived = 1;
            }
        }

        //Completing a process.
        deQueue(q);
        bubbleSort(q->front); //Sort processes according to their running time.
        num_proc_left = getCount(q->front);
        printf("%d, %s, id=%d, proc-remaining=%d\n", cum_time, FINISHED_STATE, curr_process.process_id, num_proc_left);

        //Record turnaround times and time each process completed.
        turnaround_times[total_finished] = cum_time - curr_process.time;
        time_stamps[total_finished] = cum_time;
        total_finished++; 

        //Record current overhead max and sum of overheads.
        curr_overhead = calc_overhead(turnaround_times[count], curr_process.expected_time);
        if (curr_overhead >= cs_stats.overhead[OVERHEAD_MAX]) cs_stats.overhead[OVERHEAD_MAX] = curr_overhead;
        cs_stats.overhead[OVERHEAD_AVG] = cs_stats.overhead[OVERHEAD_AVG] + curr_overhead;
        count++;

        //Jump to next process if nothing arrives in the meantime.
        if ((processes_arrived==0) & (processes[curr_process.index+1].running_time!=0)) {
            if (processes[curr_process.index+1].queue_check != 1) {
                enQueue(q, processes[curr_process.index+1]);
                cum_time = processes[curr_process.index+1].time;
                processes[curr_process.index+1].queue_check = 1;
            }
        }
    }
    //Calculate and print performance statistics.
    calc_throughput(time_stamps, no_of_processes, cum_time, &cs_stats, total_finished);
    cs_stats.turnaround_time = calc_turnaround_time(no_of_processes, turnaround_times);
    cs_stats.overhead[OVERHEAD_AVG] = cs_stats.overhead[OVERHEAD_AVG]/(double)total_finished;
    cs_stats.makespan = cum_time;
    print_stats(&cs_stats);
}
#endif