#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>

void set_cpu_affinity(pid_t pid, int cpu_core) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core, &cpuset);

    if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpuset) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }
}

void set_process_priority(int priority) {
    if (nice(priority) == -1) {
        perror("nice");
        exit(1);
    }
}

int main() {
    FILE* input_file = fopen("input.txt", "r");
    if (input_file == NULL) {
        perror("fopen");
        exit(1);
    }

    char line[100];
    int process_number, burst_time, priority, core_number;
    int max_processes = 20; // Set this to the maximum number of processes you expect

    // Create arrays to store process information
    int process_numbers[max_processes];
    int burst_times[max_processes];
    int priorities[max_processes];
    int core_numbers[max_processes];
    int process_count = 0;

    // Read and store process information from the input file
    while (fgets(line, sizeof(line), input_file) != NULL) {
        if (sscanf(line, "%d %d %d %d", &process_number, &burst_time, &priority, &core_number) != 4) {
            fprintf(stderr, "Invalid input format in the input file.\n");
            exit(1);
        }

        process_numbers[process_count] = process_number;
        burst_times[process_count] = burst_time;
        priorities[process_count] = priority;
        core_numbers[process_count] = core_number;
        process_count++;
    }

    fclose(input_file);

    // Print the process information in a table
    printf("Process Information Table:\n");
    printf("| Process Number | Burst Time | Priority | Core Number |\n");
    for (int i = 0; i < process_count; i++) {
        printf("| %-14d | %-10d | %-8d | %-11d |\n", process_numbers[i], burst_times[i], priorities[i], core_numbers[i]);
    }
    printf("\n");

    // Create and manage processes based on the stored information
    for (int i = 0; i < process_count; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            if(core_numbers[i] != -1){
		    set_cpu_affinity(getpid(), core_numbers[i]);
	    }
            if (priorities[i] != -1) {
                set_process_priority(priorities[i]);
            }

            if(core_numbers[i] != -1){
	    	printf("Process %d (PID: %d) is running on CPU %d with priority %d.\n", process_numbers[i], getpid(), sched_getcpu(), priorities[i]);
	    }
	    else{
		printf("Process %d (PID: %d) is running on Avail CPU %d with priority %d.\n", process_numbers[i], getpid(), sched_getcpu(), priorities[i]);
	    }
            time_t start_time = time(NULL);
            while (time(NULL) - start_time < burst_times[i]*10) {
                for (int j = 0; j < 75000000; j++) {
                    // Simulate some work
                }
            }
            exit(0);
        }
    }

    // Wait for all child processes to finish
    while (wait(NULL) != -1);

    return 0;
}

