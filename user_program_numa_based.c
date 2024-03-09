#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <numa.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define MAX_PROCESSES 10

struct process_info {
    pid_t pid;
    char name[32];
    int burst_time; // This will be calculated dynamically
    int priority;
    int numa_node; // Preferred NUMA node
    int tickets; // Number of lottery tickets
    time_t start_time; // Start time for turnaround time calculation
    struct rusage ru; // Resource usage information
};

void assign_tickets_based_on_numa(struct process_info *processes, int n) {
    for (int i = 0; i < n; i++) {
        processes[i].tickets = 10; // Initialize with a base number of tickets

        if (numa_available() >= 0) {
            int preferred_node = numa_preferred();
            if (processes[i].numa_node == preferred_node) {
                processes[i].tickets += 5; // Extra tickets for matching NUMA node
            }
        }
        // Adjust ticket count based on burst time
        processes[i].tickets += processes[i].burst_time / 10;
    }
}

void run_lottery(struct process_info *processes, int n, int total_tickets) {
    srand(time(NULL)); // Seed for randomness
    int winning_ticket = rand() % total_tickets + 1;
    int ticket_count = 0;

    for (int i = 0; i < n; i++) {
        ticket_count += processes[i].tickets;
        if (ticket_count >= winning_ticket) {
            printf("Process %s (PID: %d) wins the lottery\n", processes[i].name, processes[i].pid);
            break;
        }
    }
}

int main() {
    struct process_info processes[MAX_PROCESSES];
    int n = 0;
    int total_tickets = 0;

    if (numa_available() < 0) {
        printf("NUMA is not available on this system.\n");
        return -1;
    }

    // Get the number of NUMA nodes
    int num_nodes = numa_num_configured_nodes();
    printf("Number of NUMA nodes: %d\n", num_nodes);

    // Create real processes
    char *commands[] = {"ls -l", "sleep 5"};
    int num_commands = sizeof(commands) / sizeof(char *);

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            execl("/bin/sh", "sh", "-c", commands[i], NULL);
            _exit(0); // If exec fails
        } else if (pid > 0) { // Parent process
            processes[n].pid = pid;
            snprintf(processes[n].name, sizeof(processes[n].name), "%s", commands[i]);
            processes[n].start_time = time(NULL); // Record start time
            processes[n].numa_node = -1; // Preferred NUMA node not set yet
            n++;
        } else {
            perror("fork");
            return -1;
        }
    }

    // Wait for child processes to finish and calculate burst time, turnaround time, and resource usage
    for (int i = 0; i < n; i++) {
        int status;
        pid_t pid = waitpid(processes[i].pid, &status, 0);
        if (pid > 0 && WIFEXITED(status)) {
            getrusage(RUSAGE_CHILDREN, &processes[i].ru);
            processes[i].burst_time = processes[i].ru.ru_utime.tv_sec * 1000 + processes[i].ru.ru_utime.tv_usec / 1000 + processes[i].ru.ru_stime.tv_sec * 1000 + processes[i].ru.ru_stime.tv_usec / 1000; // Convert to milliseconds
            processes[i].numa_node = numa_preferred(); // Simplified for example
            time_t end_time = time(NULL);
            time_t turnaround_time = end_time - processes[i].start_time;
            printf("Process %s (PID: %d) completed. Burst Time: %d ms, Turnaround Time: %ld s\n", processes[i].name, processes[i].pid, processes[i].burst_time, turnaround_time);
        } else {
            perror("waitpid");
            return -1;
        }
    }

    assign_tickets_based_on_numa(processes, n);

    for (int i = 0; i < n; i++) {
        total_tickets += processes[i].tickets;
    }

    run_lottery(processes, n, total_tickets);

    return 0;
}
