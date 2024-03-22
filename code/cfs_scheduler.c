#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <numa.h>
#include <time.h>
#include <sys/resource.h>

#define MAX_PROCESSES 4

struct process_info {
    char name[32];
    int numa_node;
    struct timespec start_time; // For calculating precise turnaround and wait times
    pid_t pid; // Process ID after fork
};

// Function to bind a process to its preferred NUMA node
void bind_process_to_numa_node(int numa_node) {
    if (numa_available() >= 0 && numa_node >= 0) {
        struct bitmask *mask = numa_allocate_nodemask();
        numa_bitmask_setbit(mask, numa_node);
        numa_bind(mask);
        numa_free_nodemask(mask);
    }
}

// Simulate a CFS-like scheduling for NUMA-aware execution
void execute_processes_cfs(struct process_info *processes, int n) {
    for (int i = 0; i < n; ++i) {
        clock_gettime(CLOCK_MONOTONIC, &processes[i].start_time); // Start time before execution
        processes[i].pid = fork();
        if (processes[i].pid == 0) {
            // Child process: Set NUMA node affinity and execute command
            bind_process_to_numa_node(processes[i].numa_node);
            execl("/bin/sh", "sh", "-c", processes[i].name, NULL);
            _exit(EXIT_FAILURE); // Exit if exec fails
        }
    }

    // Parent process: Wait for all child processes and measure times
    for (int i = 0; i < n; ++i) {
        int status;
        struct rusage usage;
        waitpid(processes[i].pid, &status, 0); // Wait for each process to finish
        clock_gettime(CLOCK_MONOTONIC, &(processes[i].start_time)); // Re-using start_time to capture end time

        getrusage(RUSAGE_CHILDREN, &usage); // Get resource usage for child processes
        long burst_time = (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1000 + 
                          (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1000; // Convert to milliseconds
        printf("Process %s (NUMA Node: %d, PID: %d) completed.\nBurst Time: %ld ms\n",
               processes[i].name, processes[i].numa_node, processes[i].pid, burst_time);
    }
}

int main() {
    struct process_info processes[MAX_PROCESSES] = {
        {"./cpu1", 0}, // Assuming cpu1, cpu2, memory1, memory2 are your program names
        {"./cpu2", 1},
        {"./memory1", 0},
        {"./memory2", 1},
    };

    execute_processes_cfs(processes, MAX_PROCESSES);

    return 0;
}
