#ifndef PROCESS_STATS_H
#define PROCESS_STATS_H

#include "process_defs.h"

#define ONE_INTERVAL 60
#define THROUGHPUT_AVG 0
#define THROUGHPUT_MIN 1
#define THROUGHPUT_MAX 2
#define OVERHEAD_MAX 0
#define OVERHEAD_AVG 1
#define FIRST_INTERVAL 0

/* A group of functions used to aid in calculating performance statistics */

void initialise_stats(Stats_type *stats);
int calc_turnaround_time(int no_of_processes, int *turnaround_times);
double calc_overhead(int turnaround_time, int process_time);
void calc_throughput(int *time_stamps, int no_of_processes, int final_cum_time, Stats_type *stats, int total_finished);
void print_stats(Stats_type *stats);

//Function to initialise all values used in statistic calculations.
void initialise_stats(Stats_type *stats) {
    stats->throughput[THROUGHPUT_AVG]=0;
    stats->throughput[THROUGHPUT_MIN]=0;
    stats->throughput[THROUGHPUT_MAX]=0;
    stats->turnaround_time=0;
    stats->overhead[OVERHEAD_MAX]=0.0;
    stats->overhead[OVERHEAD_AVG]=0.0;
    stats->makespan=0;
}

//Function used to calculate the average turnaround time.
int calc_turnaround_time(int no_of_processes, int *turnaround_times) {
    int i=0, sum=0;

    for(i=0; i < no_of_processes; i++) {
        sum = sum + turnaround_times[i];
    }

    return (int)ceil((double)sum/(double)no_of_processes);
}

//Function used to calculate the overhead time of a process.
double calc_overhead(int turnaround_time, int process_time) {
    return (double)turnaround_time/(double)process_time;
}

//Function used to calculate the average, minimum and maximum throughput.
void calc_throughput(int *time_stamps, int no_of_processes, int final_cum_time, Stats_type *stats, int total_finished) {
    int i = 0, count = 0, completed = 0;
    int no_of_intervals = (int)(ceil((double)final_cum_time/(double)60));
    int num_completed[no_of_intervals];
    int curr_interval[NUM_INTERVALS];
    curr_interval[STARTING_INTERVAL] = 1;
    curr_interval[ENDING_INTERVAL] = 60;

    //Determine the number of processes completed for each interval throughout the span of execution.
    while(count < no_of_intervals) {
        for (i=0; i < no_of_processes; i++) {
            if ((curr_interval[STARTING_INTERVAL] <= time_stamps[i]) && (time_stamps[i] <= curr_interval[ENDING_INTERVAL])) {
                completed = completed + 1;
            }
        }
        num_completed[count] = completed;
        completed = 0;
        count++;
        curr_interval[STARTING_INTERVAL] = curr_interval[STARTING_INTERVAL] + 60;
        curr_interval[ENDING_INTERVAL] = curr_interval[ENDING_INTERVAL] + 60;
    }

    stats->throughput[THROUGHPUT_MIN] = num_completed[FIRST_INTERVAL];

    //Determine the minimum and maximum throughput.
    for(i=0; i < no_of_intervals; i++) {
        if(stats->throughput[THROUGHPUT_MIN] > num_completed[i]) stats->throughput[THROUGHPUT_MIN] = num_completed[i];
        if (stats->throughput[THROUGHPUT_MAX] < num_completed[i]) stats->throughput[THROUGHPUT_MAX] = num_completed[i]; 
    }

    //Calculate the average throughput.
    int no_of_intervals_floor = (int)floor(final_cum_time/60);
    stats->throughput[THROUGHPUT_AVG] = total_finished/no_of_intervals_floor;
}

//Function to print performance statistics.
void print_stats(Stats_type *stats) {
    printf("Throughput %d, %d, %d\n", stats->throughput[THROUGHPUT_AVG], stats->throughput[THROUGHPUT_MIN], stats->throughput[THROUGHPUT_MAX]);
    printf("Turnaround time %d\n", stats->turnaround_time);
    printf("Time overhead %.2f %.2f\n", stats->overhead[OVERHEAD_MAX], stats->overhead[OVERHEAD_AVG]);
    printf("Makespan %d\n", stats->makespan);
}
#endif