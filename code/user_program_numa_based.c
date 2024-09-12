

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <numa.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define MAX_PROCESSES 4

struct process_info {
    char name[32];
    int tickets; // Number of lottery tickets
    int numa_node; // Preferred NUMA node
    time_t start_time; // To calculate wait and turnaround times
    struct rusage usage; // To calculate burst time
};

void assign_tickets_and_numa_nodes(struct process_info *processes, int n) {
    for (int i = 0; i < n; i++) {
        processes[i].tickets = 10 + i * 5; // Increasing ticket numbers
        processes[i].numa_node = i % numa_num_configured_nodes(); // Rotate through available NUMA nodes
    }
}

int run_lottery(struct process_info *processes, int n) {
    int total_tickets = 0;
    for (int i = 0; i < n; i++) {
        total_tickets += processes[i].tickets;
    }

    srand(time(NULL)); // Seed for randomness
    int winning_ticket = rand() % total_tickets;
    int ticket_count = 0;

    for (int i = 0; i < n; i++) {
        ticket_count += processes[i].tickets;
        if (ticket_count > winning_ticket) {
            return i; // Return index of the winning process
        }
    }
    return -1; 
}

void execute_process(int winner_index, struct process_info *processes) {
    printf("Process %s wins the lottery and will now execute.\n", processes[winner_index].name);
    
    // Capture start time
    processes[winner_index].start_time = time(NULL);
    
    pid_t pid = fork();
    if (pid == 0) { // Child process
        // Apply NUMA binding for the child process if NUMA is available
        if (numa_available() >= 0) {
            struct bitmask *mask = numa_allocate_nodemask();
            numa_bitmask_setbit(mask, processes[winner_index].numa_node);
            numa_bind(mask);
            numa_free_nodemask(mask);
        }

        execl("/bin/sh", "sh", "-c", processes[winner_index].name, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else if (pid > 0) { // Parent process
        int status;
        waitpid(pid, &status, 0);
        
        // Calculate and display metrics after process completion
        getrusage(RUSAGE_CHILDREN, &processes[winner_index].usage);
        time_t end_time = time(NULL);

        long burst_time = (processes[winner_index].usage.ru_utime.tv_sec + processes[winner_index].usage.ru_stime.tv_sec) * 1000 +
                          (processes[winner_index].usage.ru_utime.tv_usec + processes[winner_index].usage.ru_stime.tv_usec) / 1000;
        time_t turnaround_time = end_time - processes[winner_index].start_time;
        

        printf("Burst Time: %ld ms, Turnaround Time: %ld s\n", burst_time, turnaround_time);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

int main() {
    if (numa_available() < 0) {
        printf("NUMA is not available on this system.\n");
    } else {
        printf("Number of NUMA nodes: %d\n", numa_num_configured_nodes());
    }

    struct process_info processes[MAX_PROCESSES] = {
        {"./cpu1", 0, -1},
        {"./cpu2", 0, -1},
        {"./memory1", 0, -1},
        {"./memory2", 0, -1},
    };

    assign_tickets_and_numa_nodes(processes, MAX_PROCESSES);
    int winner_index = run_lottery(processes, MAX_PROCESSES);

    if (winner_index != -1) {
        execute_process(winner_index, processes);
    } else {
        printf("Failed to select a winning process.\n");
    }

    return 0;
}
