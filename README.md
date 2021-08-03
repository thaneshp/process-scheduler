# Process Scheduler

Process Scheduler is a program built as part of my Computer Systems (COMP30023) assignment. The program is used to simulate Process and Memory Management scheduling in an OS. Basically, the program works by inputting a sample list of processes with chosen commands, outputting a series of sequences describing the actions taken by the scheduler.  

## Algorithms Used

### Scheduling Algorithms

1. First-come first-served (ff)
    - Executes processes in order of their arrival.
    - Continues executing until the total running time reaches the specified job-time.
2. Round-robin (rr)
    - Every process is executed on the CPU for a limited amount of time. 
    - Once this time has elapsed, the CPU is given to the next process in the queue and the current process is placed at the end.
    - This sequence repeats until all processes have finished executing.
3. Shortest Job First (cs)
    - Compares all the processes that have arrived and sorts them based on their running time.
    - It then executes the processes with the shortest running time first.

### Memory Management Algorithms

1. Swapping-X (p)
    - A method used to swap pages of processes between main memory and the disk.
    - A process can execute if all the pages it requires is in main memory, otherwise another process must be swapped out to make space.
2. Virtual memory 
    - A method used to mimic the availability of larger memory.
    1. Least Recently Used (v)
        - Evicts the pages of the processes which was least recently executed.
    2. Most Recently Used (cm)
        - Evicts the pages of the processes which was most recently executed.

**Note**: The letters given in the parenthesis are commands to be used in the execution of the program. More details given below.

## Setup with Docker

If you have Docker installed, use the following command to pull the Process Scheduler's image. Alternatively, if you are don't have Docker, skip this step and set up the project via cloning.

```bash
$ docker pull thaneshp/process-scheduler
```

## Usage with Docker

Once you have pulled the image, you can use the following command to run the Process Scheduler. The options shown here are explained further below.

```bash
$ docker run -it --rm thaneshp/process-scheduler -f <filename> -a <scheduling-algorithm> -m <memory-allocation> -s <memory-size> -q <quantum>
```

### Example

Using docker to execute a First-come first-serve algorithm using Swapping-X algorithm with 20KB memory.

```bash
docker run -it --rm thaneshp/process-scheduler -f input-data/rr_memoryoverlap.txt -a ff -s 20 -m p
```

For more examples, review the examples section below.

## Clone

Clone this repo to your local machine using https://github.com/thaneshp/process-scheduler.git

## Setup

Using terminal. CD into the project folder and type in the following command to install.

```bash
$ make
```

You may also use this command to remove any of the installed executables.

```bash
$ make clean
```

## Usage

Here is how a typical input should be formatted. The order shown here is unimportant.

```bash
$ ./scheduler -f <filename> -a <scheduling-algorithm> -m <memory-allocation> -s <memory-size> -q <quantum>
```

### Arguments
- \-f ***filename*** will specify the name of file describing the processes.
- \-a ***scheduling-algorithm*** where you can choose from one of the scheduling algorithms given above. {ff, rr, cs}
- \-m ***memory allocation*** where you can choose from {u,p,v,cm} where **u** indicates unlimited memory.
- \-s ***memory-size*** where memory-size is an integer indicating the size of memory in KB. This option can be ignored in the case of unlimited memory. i.e., when **-m u**
- \-q ***quantum*** where quantum is an integer (in seconds) used to specify the time each process should run when using round-robin algorithm.


## Examples

1. First-come first-serve algorithm using unlimited memory.
```bash
$ ./scheduler -f input-data/processes.txt -a ff -m u
```

2. Round-robin algorithm, allocated 10 seconds quantum time and using unlimited memory.
```bash
$ ./scheduler -f input-data/processes.txt -a rr -m u -q 10
```

3. First-come first-serve algorithm using Swapping-X algorithm with 20KB memory.
```bash
$ ./scheduler -f input-data/rr_memoryoverlap.txt -a ff -s 20 -m p
```

4. Round-robin algorithm, allocated 10 seconds quantum time utilsing Swapping-X algorithm and 20KB memory.
```bash
$ ./scheduler -f input-data/rr_memoryoverlap.txt -a rr -s 20 -m p -q 10
```

5. First-come first-serve algorithm utilising Swapping-X algorithm and 20KB memory.
```bash
$ ./scheduler -f input-data/rr_memoryoverlap.txt -a ff -s 20 -m p
```

6. Round-robin algorithm, allocated 10 seconds quantum time utilising Least Recently Used virtual memory algorithm and 20KB memory.
```bash
$ ./scheduler -f input-data/rr_memoryoverlap.txt -a rr -s 20 -m v -q 10
```

7. First-come first-serve algorithm utilising Swapping-X algorithm and 48KB memory.
```bash
$ ./scheduler -f input-data/timejumps.txt -a ff -s 48 -m p
```

8. Round-robin algorithm allocated 10 seconds quantum time utilising Swapping-X algorithm and 100KB memory.
```bash
$ ./scheduler -f input-data/timejumps.txt -a rr -s 100 -m p -q 10
```

9. Round-robin algorithm, allocated 10 seconds quantum time utilising Least Recently Used virtual memory algorithm and 100KB memory.
```bash
$ ./scheduler -f input-data/timejumps.txt -a rr -s 100 -m v -q 10
```

10. Round-robin algorithm, allocated 10 seconds quantum time utilising Swapping-X algorithm and 40KB memory.
```bash
$ ./scheduler -f input-data/rr_memoryoverlap.txt -a rr -s 40  -m p -q 10
```

11. Round-robin algorithm, allocated 10 seconds quantum time utilising Least Recently Used virtual memory algorithm and 40KB memory.
```bash
$ ./scheduler -f input-data/rr_memoryoverlap.txt -a rr -s 40 -m v -q 10
```

## Support

You can find me at any of the following places!

- Website: https://thanesh.io/
- Email: thanesh.pannirselvam@gmail.com
- LinkedIn: linkedin.com/in/thanesh-pannirselvam




