#ifndef RR_ALGORITHM_H
#define RR_ALGORITHM_H

#include "process_defs.h"
#include "linked_list_queue.h"
#include "virtual_mem_LRU.h"
#include "virtual_mem_MRU.h"

void rr_algorithm(int no_of_processes, Process_type *process, int quantum_time, char *memory, int memory_size);
int check_for_next_process(struct Queue *q, Process_type *process, int no_of_processes);
int calc_sum_usage(Process_type *in_memory, int no_of_processes);
int arrived_processes(struct Queue* q, Process_type *process, int no_of_processes, int cum_time);

void rr_algorithm(int no_of_processes, Process_type *process, int quantum_time, char *memory, int memory_size) {
    //Variables used for base round-robin algorithm.
    int cum_time = 0, pages_needed = 0, load_time, total_finished = 0;

    //Variables used for swapping-x memory algorithm.
    int mem_usage = 0, total_mem_capacity = memory_size/PAGE_SIZE, curr_available = 0, space_check = 0; 
    int processes_arrived = 0, being_used = 0, last_assigned = 0, in_mem_count = 0, sum_usage = 0;
    Process_type in_memory[no_of_processes];
    reset_in_mem(in_memory, no_of_processes);

    //Variables used for virtual memory LRU and CM algorithm.
    int pages_needed_next = process[FIRST_PROCESS].memory_size/PAGE_SIZE;
    int pages_in_mem = 0, no_of_pages = memory_size/PAGE_SIZE;
    Page_type mem_array[no_of_pages];
    initialse_memory(mem_array, no_of_pages);

    //Variables used for statistic calculation.
    int turnaround_times[no_of_processes], time_stamps[no_of_processes], count=0;
    double curr_overhead = 0;
    Stats_type rr_stats;
    initialise_stats(&rr_stats);

    //Create a queue and insert the first process.
    struct Queue* q = createQueue();
    enQueue(q, process[0]);
    process[0].queue_check = 1;

    //Execute process for quantum time as long as the queue isn't empty.
    while (!((q->front == NULL) && (q->rear == NULL))) {
        Process_type curr_process = q->front->process;
        if (strcmp(memory, "u")==0) printf("%d, %s, id=%d, remaining-time=%d\n", cum_time, RUNNING_STATE, curr_process.process_id, curr_process.running_time); 
        else if (strcmp(memory, "p")==0) printf("%d, %s, id=%d, remaining-time=%d", cum_time, RUNNING_STATE, curr_process.process_id, curr_process.running_time);
        else if (strcmp(memory, "v")==0) {
            //Determine how many pages of current process is already in memory.
            pages_in_mem = calc_process_in_memory(curr_process.process_id, no_of_pages, mem_array);
            pages_in_mem = pages_in_mem + pages_needed_next;
            pages_needed = curr_process.memory_size/PAGE_SIZE;
            //Add necessary amount of time penalty for every page not in memory.
            if (pages_in_mem < pages_needed) {
                int penalty = pages_needed - pages_in_mem;
                curr_process.running_time = curr_process.running_time + penalty;
            }
            printf("%d, %s, id=%d, remaining-time=%d", cum_time, RUNNING_STATE, curr_process.process_id, curr_process.running_time);
        } else if (strcmp(memory, "cm")==0) {
            //Determine how many pages of current process is already in memory.
            pages_in_mem = calc_process_in_memory(curr_process.process_id, no_of_pages, mem_array);
            pages_in_mem = pages_in_mem + pages_needed_next;
            pages_needed = curr_process.memory_size/PAGE_SIZE;
            //Add necessary amount of time penalty for every page not in memory.
            if (pages_in_mem < pages_needed) {
                int penalty = pages_needed - pages_in_mem;
                curr_process.running_time = curr_process.running_time + penalty;
            }
            printf("%d, %s, id=%d, remaining-time=%d", cum_time, RUNNING_STATE, curr_process.process_id, curr_process.running_time);
        }

        //Running a process using swapping-x algorithm.
        if (strcmp(memory, "p")==0) {
            //Determine whether current process is memory and no processes have arrived in the meantime.
            if ((curr_process.in_mem == 1) & (processes_arrived == 0)) {
                load_time = 0;
                sum_usage = calc_sum_usage(in_memory, no_of_processes);
                mem_usage = calc_mem_usage(sum_usage, memory_size);
                printf(", load-time=%d, mem-usage=%d%%, mem-addresses=[%s]\n", load_time, mem_usage, curr_process.mem_addresses);
            } else {
                //Determine amount of memory needed for process to be executed.
                load_time = calc_load_time(curr_process.memory_size);
                pages_needed = curr_process.memory_size/PAGE_SIZE;
                being_used = being_used + pages_needed;
                curr_available = total_mem_capacity - being_used;
                
                //Assign necessary amount of pages this process requires.
                curr_process.mem_addresses = malloc((pages_needed * sizeof(char)) + (pages_needed - 1));
                memset(curr_process.mem_addresses, 0, (pages_needed * sizeof(char)) + (pages_needed - 1));
                last_assigned = assign_addresses(curr_process.mem_addresses, pages_needed, last_assigned);
                curr_process.start_page = last_assigned - pages_needed;

                //Indicate that the current process is stored in memory.
                curr_process.in_mem = 1;
                curr_process.mem_arrival_time = cum_time;
                in_memory[curr_process.index] = curr_process;
                in_mem_count++;

                sum_usage = calc_sum_usage(in_memory, no_of_processes);
                mem_usage = calc_mem_usage(sum_usage, memory_size);
                printf(", load-time=%d, mem-usage=%d%%, mem-addresses=[%s]\n", load_time, mem_usage, curr_process.mem_addresses);
            }
        }
       
        //Running a process using virtual memory LRU algorithm.
        if (strcmp(memory, "v")==0) {
            //Determine whether all pages of process is currently in memory.
            int curr_in_mem = calc_process_in_memory(curr_process.process_id, no_of_pages, mem_array);
            if (curr_in_mem == (curr_process.memory_size/PAGE_SIZE)) curr_process.in_mem = 1;
            else curr_process.in_mem=0;
            //Execute process based on whether other processes have arrived in the meantime.
            if ((curr_process.in_mem == 1) & (processes_arrived == 0)) {
                load_time = 0;
                being_used = calc_pages_in_mem(mem_array, no_of_pages);
                mem_usage = calc_mem_usage(being_used, no_of_pages);
                printf(", load-time=%d, mem-usage=%d%%, mem-addresses=[%s]\n", load_time, mem_usage, curr_process.mem_addresses);
            } else { 
                //Determine amount of memory required to execute current process.
                curr_process.start_time = cum_time;
                load_time = calc_load_time(PAGE_SIZE*pages_needed_next);
                pages_needed_next = pages_in_mem;
                pages_needed = pages_needed_next;

                //Assign necessary amount of pages to undertake this process.
                assign_pages(mem_array, pages_needed, curr_process.process_id, cum_time, no_of_pages);
                being_used = calc_pages_in_mem(mem_array, no_of_pages);
                mem_usage = calc_mem_usage(being_used, no_of_pages);
                curr_process.mem_addresses = malloc((pages_needed * sizeof(char)) + (pages_needed - 1));
                memset(curr_process.mem_addresses, 0, (pages_needed * sizeof(char)) + (pages_needed - 1));
                generate_mem_addresses(mem_array, no_of_pages, &curr_process, pages_needed, curr_process.mem_addresses);
                printf(", load-time=%d, mem-usage=%d%%, mem-addresses=[%s]\n", load_time, mem_usage, curr_process.mem_addresses);
            }
        }

        //Running a process using virtual memory MRU algorithm.
         if (strcmp(memory, "cm")==0) {
            //Determine whether all pages of process is currently in memory.
            int curr_in_mem = calc_process_in_memory(curr_process.process_id, no_of_pages, mem_array);
            if (curr_in_mem == (curr_process.memory_size/PAGE_SIZE)) curr_process.in_mem = 1;
            else curr_process.in_mem=0;
            //Execute process based on whether other processes have arrived in the meantime.
            if ((curr_process.in_mem == 1) & (processes_arrived == 0)) {
                load_time = 0;
                being_used = calc_pages_in_mem(mem_array, no_of_pages);
                mem_usage = calc_mem_usage(being_used, no_of_pages);
                printf(", load-time=%d, mem-usage=%d%%, mem-addresses=[%s]\n", load_time, mem_usage, curr_process.mem_addresses);
            } else {
                //Determine amount of memory required to execute current process.
                curr_process.start_time = cum_time;
                load_time = calc_load_time(PAGE_SIZE*pages_needed_next);
                pages_needed_next = pages_in_mem;
                pages_needed = pages_needed_next;

                //Assign necessary amount of pages to undertake this process.
                assign_pages(mem_array, pages_needed, curr_process.process_id, cum_time, no_of_pages);
                being_used = calc_pages_in_mem(mem_array, no_of_pages);
                mem_usage = calc_mem_usage(being_used, no_of_pages);
                curr_process.mem_addresses = malloc((pages_needed * sizeof(char)) + (pages_needed - 1));
                memset(curr_process.mem_addresses, 0, (pages_needed * sizeof(char)) + (pages_needed - 1));
                generate_mem_addresses(mem_array, no_of_pages, &curr_process, pages_needed, curr_process.mem_addresses);
                printf(", load-time=%d, mem-usage=%d%%, mem-addresses=[%s]\n", load_time, mem_usage, curr_process.mem_addresses);
            }
        }
        
        //Determine whether memory is full. Used when evicting memory for swapping-x algorithm.
        if (curr_available > 0) space_check = 0;
        else space_check = 1;

        //Determine whether current process has a shorter execution time left than quantum time.
        if (curr_process.running_time < quantum_time) {
            cum_time = cum_time + curr_process.running_time;
            curr_process.running_time = curr_process.running_time - curr_process.running_time;
        } else {
            curr_process.running_time = curr_process.running_time - quantum_time;
            cum_time = cum_time + quantum_time;
        }
        
        //Check whether processes have arrived when memory is unlimited.
        if (strcmp(memory, "u")==0) processes_arrived = arrived_processes(q, process, no_of_processes, cum_time);

        //Evicting memory used for swapping-x algorithm.
        if (strcmp(memory, "p")==0) {
            cum_time = cum_time + load_time;
            processes_arrived = arrived_processes(q, process, no_of_processes, cum_time);
            //Evict a process when other processes are waiting and/or there is no memory available.
            if ((processes_arrived == 1) & (space_check == 1)) {
                last_assigned = evict_process(cum_time, &curr_process, in_memory);
                in_mem_count--;
            } else if ((q->rear != NULL) & (space_check == 1)) {
                int evicted_process_index;
                evicted_process_index = check_min_arrival_time(in_memory, no_of_processes, in_mem_count);
                //Determine whether current process is finished or has already been evicted.
                if (curr_process.running_time == 0) {
                    last_assigned = evict_process(cum_time, &curr_process, in_memory);
                    in_mem_count--;
                    being_used = being_used - pages_needed;
                } else if (curr_process.process_id == in_memory[evicted_process_index].process_id) {
                    last_assigned = evict_process(cum_time, &curr_process, in_memory);
                    in_mem_count--;
                    being_used = being_used - pages_needed;
                } else if (q->front->next->process.process_id == in_memory[evicted_process_index].process_id) {
                    q->front->next->process.in_mem=1;
                } else { 
                    Process_type *process;
                    process = search(q->front, in_memory[evicted_process_index].process_id);
                    last_assigned = evict_process(cum_time, process, in_memory);
                    in_mem_count--;
                    being_used = being_used - pages_needed;
                }
            //Evict process when running time is complete.
            } else if (curr_process.running_time == 0) {
                last_assigned = evict_process(cum_time, &curr_process, in_memory);
                in_mem_count--;
                being_used = being_used - pages_needed;
            } else {
                curr_process.in_mem=1;
            }
        }
        
        //Evicting memory used for virtual memory LRU algorithm.
        if (strcmp(memory, "v")==0) {
            int next_process_id = -1, evict_check = 0, no_available = 0;
            cum_time = cum_time + load_time;
            processes_arrived = arrived_processes(q, process, no_of_processes, cum_time);
            no_available = no_of_pages - being_used;
            //Determine how many pages the next process will need.
            if (q->front->next != NULL) {
                pages_needed_next = q->front->next->process.memory_size/PAGE_SIZE;
                next_process_id = q->front->next->process.process_id;
            }
            //Determine how many pages need to be evicted so that next process can execute.
            if (no_available < pages_needed_next) {
                int next_pages_in_mem = calc_process_in_memory(next_process_id, no_of_pages, mem_array);
                int pages_for_loading = pages_needed_next - next_pages_in_mem;
                if (no_available < pages_for_loading) {
                    if (curr_process.running_time > 0) pages_needed_next = evict_pages_LRU(mem_array, no_of_pages, pages_needed_next, curr_process.start_time, cum_time, next_process_id, no_available);
                    else {
                        pages_needed_next = evict_pages_finished(mem_array, no_of_pages, &curr_process, cum_time, &q->front->next->process);
                        curr_process.evicted = 1;
                    }
                    evict_check = 1;
                }
                if (evict_check==0) pages_needed_next = pages_for_loading;
            }
            if (curr_process.evicted == 1) free(curr_process.mem_addresses);
        }

        //Evicting memory used for virtual memory MRU algorithm.
        if (strcmp(memory, "cm")==0) {
            int next_process_id = -1, no_available = 0, evict_check=0;
            cum_time = cum_time + load_time;
            processes_arrived = arrived_processes(q, process, no_of_processes, cum_time);
            no_available = no_of_pages - being_used;
            //Determine how many pages the next process will need.
            if (q->front->next != NULL) {
                pages_needed_next = q->front->next->process.memory_size/PAGE_SIZE;
                next_process_id = q->front->next->process.process_id;
            }
            //Determine how many pages need to be evicted so that next process can execute.
            if (no_available < pages_needed_next) {
                int next_pages_in_mem = calc_process_in_memory(next_process_id, no_of_pages, mem_array);
                int pages_for_loading = pages_needed_next - next_pages_in_mem;
                if (no_available < pages_for_loading) {
                    if (curr_process.running_time > 0) pages_needed_next = evict_pages_MRU(mem_array, no_of_pages, pages_needed_next, cum_time, next_process_id, no_available);
                    else {
                        pages_needed_next = evict_pages_finished(mem_array, no_of_pages, &curr_process, cum_time, &q->front->next->process);
                        curr_process.evicted = 1;
                    }
                    evict_check =1;
                }
                if (evict_check==0) pages_needed_next = pages_for_loading;
            } 
            if (curr_process.evicted == 1) free(curr_process.mem_addresses);   
        }
       
        //Determine whether current process can be finished or still needs to be completed.
        if (curr_process.running_time > 0) {
            deQueue(q);
            enQueue(q, curr_process);
        } else {
            //Record turnaround times and time each process took to complete.
            turnaround_times[total_finished] = cum_time - curr_process.time;
            time_stamps[total_finished] = cum_time;
            total_finished++;

            //Record current overhead max and sum of overheads.
            curr_overhead = calc_overhead(turnaround_times[count], curr_process.expected_time);
            if (curr_overhead >= rr_stats.overhead[OVERHEAD_MAX]) rr_stats.overhead[OVERHEAD_MAX] = curr_overhead;
            rr_stats.overhead[OVERHEAD_AVG] = rr_stats.overhead[OVERHEAD_AVG] + curr_overhead;
            count++;

            //Completing a process
            deQueue(q);
            //Determine whether a process should be evicted from memory before finalising.
            if ((strcmp(memory,"v")==0 || strcmp(memory,"cm")==0) & (curr_process.evicted!=1)) {
                if (getCount(q->front) == 0) evict_pages_finished(mem_array, no_of_pages, &curr_process, cum_time, NULL);
                else evict_pages_finished(mem_array, no_of_pages, &curr_process, cum_time, &q->front->next->process);
                free(curr_process.mem_addresses);
            }
            printf("%d, %s, id=%d, proc-remaining=%d\n", cum_time, FINISHED_STATE, curr_process.process_id, getCount(q->front));
        }

        //Jump to next process if nothing has arrived since elapsed time.
        int prev_cum_time = cum_time;
        if (((q->front == NULL) && (q->rear == NULL))) {
            cum_time = check_for_next_process(q, process, no_of_processes);
            if (cum_time == 0) cum_time = prev_cum_time;
            if (getCount(q->front) > 0) pages_needed_next = q->front->process.memory_size/PAGE_SIZE;
        }
    }
    //Calculate and print performance statistics.
    calc_throughput(time_stamps, no_of_processes, cum_time, &rr_stats, total_finished);
    rr_stats.turnaround_time = calc_turnaround_time(no_of_processes, turnaround_times);
    rr_stats.overhead[OVERHEAD_AVG] = rr_stats.overhead[OVERHEAD_AVG]/(double)total_finished;
    rr_stats.makespan = cum_time;
    print_stats(&rr_stats);
}

//Function used to check if there are remaining processes to be completed if they haven't already arrived.
int check_for_next_process(struct Queue *q, Process_type *process, int no_of_processes) {
    int i=0, new_cum_time = 0;

    for (i=0; i < no_of_processes; i++) {
        if (process[i].queue_check != 1) {
            enQueue(q, process[i]);
            process[i].queue_check = 1;
            new_cum_time = process[i].time;
            break;
        }
    }
    return new_cum_time;
}

//Function used to calculate the total amount of memory being used.
int calc_sum_usage(Process_type *in_memory, int no_of_processes) {
    int i = 0, sum_usage = 0;

    for (i=0; i < no_of_processes; i++) {
        if (in_memory[i].in_mem == 1) {
            sum_usage = sum_usage + in_memory[i].memory_size;
        }
    }
    return sum_usage;
}

//Function to determine whether a process has arrived up to a given time.
int arrived_processes(struct Queue* q, Process_type *process, int no_of_processes, int cum_time) {
    int i = 0, processes_arrived = 0;
    
    //Add processes that have arrived in the meantime to the end of the queue.
    for (i=0; i < no_of_processes; i++) {
        if ((process[i].queue_check != 1) && (process[i].time <= cum_time)) {
            enQueue(q, process[i]);
            process[i].queue_check = 1;
            processes_arrived = 1;
        }
    }
    return processes_arrived;
}
#endif