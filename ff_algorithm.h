#ifndef FF_ALGORITHM_H
#define FF_ALGORITHM_H

#include "process_defs.h"
#include "mem_swapping.h"
#include "process_stats.h"
#include "linked_list_queue.h"

void ff_algorithm(int no_of_processes, Process_type *process, char *memory, int memory_size);

void ff_algorithm(int no_of_processes, Process_type *process, char *memory, int memory_size) {
    int cum_time = 0, num_proc_left = 0;
    
    //Variables for swapping-x memory algorithm.
    int load_time = 0, mem_usage = 0, pages_needed = 0;
    char *mem_addresses;

    //Variables for statistic calculations.
    int total_finished = 0;
    int time_stamps[no_of_processes];
    int turnaround_times[no_of_processes];
    double curr_overhead=0.0;
    Stats_type ff_stats;
    initialise_stats(&ff_stats);

    //Create queue and place first process in.
    struct Queue* q = createQueue();
    enQueue(q, process[FIRST_PROCESS]);
    process[FIRST_PROCESS].queue_check = 1;

    while (!((q->front == NULL) && (q->rear == NULL))) {
        Process_type curr_process = q->front->process;
        if (strcmp(memory, "u")==0) printf("%d, %s, id=%d, remaining-time=%d\n", cum_time, RUNNING_STATE, curr_process.process_id, curr_process.running_time);
        else printf("%d, %s, id=%d, remaining-time=%d", cum_time, RUNNING_STATE, curr_process.process_id, curr_process.running_time); 
        cum_time = cum_time + curr_process.running_time;

        //Run chosen memory management algorithm.
        if ((strcmp(memory, "p")==0) || (strcmp(memory,"v")==0)) {
            //Determine amount of memory needed for process to be executed.
            load_time = calc_load_time(curr_process.memory_size);
            mem_usage = calc_mem_usage(curr_process.memory_size, memory_size);
            pages_needed = curr_process.memory_size/PAGE_SIZE;

            //Assign necessary amount of pages for this process.
            mem_addresses = malloc((pages_needed * sizeof(char)) + (pages_needed - 1));
            memset(mem_addresses, 0, (pages_needed * sizeof(char)) + (pages_needed - 1));
            assign_addresses(mem_addresses, pages_needed, START_PAGE);
            printf(", load-time=%d, mem-usage=%d%%, mem-addresses=[%s]\n", load_time, mem_usage, mem_addresses);

            //Evict memory used by this process.
            cum_time = cum_time + load_time;
            printf("%d, %s, mem-addresses=[%s]\n", cum_time, EVICTED_STATE, mem_addresses);
            free(mem_addresses);  
        }

        //Add processes that have arrived in the meantime to the end of the queue.
        int i = 0, processes_arrived = 0;
        for (i=0; i < no_of_processes; i++) {
            if ((process[i].queue_check != 1) && (process[i].time <= cum_time)) {
                process[i].queue_check = 1;
                enQueue(q, process[i]);
                processes_arrived = 1;
            }
        }

        //Completing a process.
        deQueue(q);
        num_proc_left = getCount(q->front);
        printf("%d, %s, id=%d, proc-remaining=%d\n", cum_time, FINISHED_STATE, curr_process.process_id, num_proc_left);

        //Record turnaround times and time each process completed.
        turnaround_times[total_finished] = cum_time - curr_process.time;
        time_stamps[total_finished] = cum_time;
        total_finished++;
        
        //Record current overhead max and sum of overheads.
        curr_overhead = calc_overhead(turnaround_times[curr_process.index], curr_process.running_time);
        if (curr_overhead >= ff_stats.overhead[OVERHEAD_MAX]) ff_stats.overhead[OVERHEAD_MAX] = curr_overhead;
        ff_stats.overhead[OVERHEAD_AVG] = ff_stats.overhead[OVERHEAD_AVG] + curr_overhead;

        //Jump to next process if nothing arrives in the meantime.
        if ((processes_arrived==0) & (process[curr_process.index+1].running_time!=0)) {
            if (process[curr_process.index+1].queue_check != 1) {
                enQueue(q, process[curr_process.index+1]);
                cum_time = process[curr_process.index+1].time;
                process[curr_process.index+1].queue_check = 1;
            }
        }
    }
    //Calculate and print performance statistics.
    calc_throughput(time_stamps, no_of_processes, cum_time, &ff_stats, total_finished);
    ff_stats.turnaround_time = calc_turnaround_time(no_of_processes, turnaround_times);
    ff_stats.overhead[OVERHEAD_AVG] = ff_stats.overhead[OVERHEAD_AVG]/(double)total_finished;
    ff_stats.makespan = cum_time;
    print_stats(&ff_stats);
}
#endif