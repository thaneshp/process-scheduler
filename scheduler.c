/* Student Name: Thaneshmaran Pannirselvam 
   Student ID: 912363 
   Subject: COMP30023
   Description: This program is used to simulate Process and Memory Management Scheduling. 
                It works by entering a list of processes with chosen commands to evaluate
                how processes are managed in memory.
*/

#include "process_defs.h"
#include "ff_algorithm.h"
#include "rr_algorithm.h"
#include "cs_algorithm.h"

int countprocesses(FILE *fp);
void store_process_values(FILE *fp, Process_type *process);
void reorder_processes(Process_type *processes, int no_of_processes);

int main(int argc, char **argv) {
    if (argc < 7) {
        printf("usage %s -f <file_name.txt> -a <scheduling-algo> -m <mem-algo> -s <mem-size> -q <quantum>\n", argv[0]);
        exit(0);
    }

    int no_of_processes = 0, i=0;
    Process_type* process;
    Command_type commands;
    commands.mem_size = -1;
    commands.quant_time = -1;
    
    //Determine and store user comments in struct.
    for (i=0; i < argc; i++) {
        if (strcmp(argv[i],"-f")==0) {
            FILE *fp = fopen(argv[i+1],"r");
            no_of_processes = countprocesses(fp);
            rewind(fp);
            process = malloc(no_of_processes * sizeof(Process_type));
            store_process_values(fp, process);
            fclose(fp);
        } else if (strcmp(argv[i],"-m")==0) {
            strcpy(commands.memory, argv[i+1]);
        } else if (strcmp(argv[i], "-a")==0) {
            strcpy(commands.schedule, argv[i+1]);
        } else if (strcmp(argv[i], "-s")==0) {
            commands.mem_size = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "-q")==0) {
            commands.quant_time = atoi(argv[i+1]);
        }
    }

    //Assign default values for quantum and memory size if none assigned by user.
    if (commands.quant_time == -1) commands.quant_time = 10;
    if (commands.mem_size == -1) commands.mem_size = 0;

    //Re-order processes where arrival time is the same.
    reorder_processes(process, no_of_processes);

    //Execute program depending on chosen scheduling algorithm.
    if (strcmp(commands.schedule, "ff")==0) {
        ff_algorithm(no_of_processes, process, commands.memory, commands.mem_size);
    } else if (strcmp(commands.schedule, "rr")==0) {
        rr_algorithm(no_of_processes, process, commands.quant_time, commands.memory, commands.mem_size);
    } else if (strcmp(commands.schedule, "cs")==0) {
        cs_algorithm(no_of_processes, process, commands.memory, commands.mem_size);
    }
     
    free(process);
   
}

//Count the number of processes in a file.
int countprocesses(FILE *fp) {
    int ch=0;
    int no_of_processes = 0;

    //Check if file is empty.
    if (fp == NULL) {
        return 0;
    }
    
    //Determine how many lines the file has.
    no_of_processes++;
    while(!feof(fp)) {
        ch = fgetc(fp);
        if(ch == '\n') {
            no_of_processes++;
        }
    }

    return no_of_processes;
}

//Store the process values in an structured array.
void store_process_values(FILE *fp, Process_type *process) {
    int x = 0;
    int time, processes, memory_size, running_time;
    
    //Scan processes and store corresponding values in respective struct.
    while (fscanf(fp,"%d %d %d %d", &time, &processes, &memory_size, &running_time) != EOF) {
        process[x].time = time;
        process[x].process_id = processes;
        process[x].memory_size = memory_size;
        process[x].running_time = running_time;
        process[x].expected_time = running_time;
        process[x].index = x;
        process[x].mem_arrival_time = 0;
        process[x].in_mem = 0;
		x++;
	}
}

//Re-order processes if two processes have same arrival time.
void reorder_processes(Process_type *processes, int no_of_processes) {
    int i=0;

    //Check if two processes have same arrival time and swap order depending on value of process ID.
    for(i=0; i<no_of_processes; i++) {
        Process_type temp;
        if ((processes[i].time == processes[i+1].time) & (processes[i+1].process_id < processes[i].process_id)) {
            temp = processes[i+1];
            processes[i+1] = processes[i];
            processes[i] = temp;
        }
    }
}

