#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sched.h>

#define MAX_PROCESSES 4

struct process_info {
    char name[32];
    int tickets; // Number of lottery tickets
    struct timespec start_time; // For calculating precise turnaround and wait times
};

void assign_tickets(struct process_info *processes, int n) {
    for (int i = 0; i < n; i++) {
        
        processes[i].tickets = 10 + i * 10;
    }
}

int run_lottery(struct process_info *processes, int n) {
    int total_tickets = 0;
    for (int i = 0; i < n; i++) {
        total_tickets += processes[i].tickets;
    }

    srand(time(NULL));
    int winning_ticket = rand() % total_tickets;
    int ticket_count = 0;

    for (int i = 0; i < n; i++) {
        ticket_count += processes[i].tickets;
        if (ticket_count > winning_ticket) {
            return i; // Index of the winning process
        }
    }
    return -1;
}

void execute_process(int winner_index, struct process_info *processes, struct timespec program_start_time) {
    printf("Process %s wins the lottery and will now execute.\n", processes[winner_index].name);

    pid_t pid = fork();
    struct timespec exec_start_time, exec_end_time;
    if (pid == 0) { // Child process
        execl("/bin/sh", "sh", "-c", processes[winner_index].name, NULL);
        _exit(EXIT_FAILURE); 
    } else if (pid > 0) { // Parent process
        clock_gettime(CLOCK_MONOTONIC, &exec_start_time); 
        wait(NULL); // Wait for the child process to finish

        clock_gettime(CLOCK_MONOTONIC, &exec_end_time); // Mark execution end time

        struct rusage usage;
        getrusage(RUSAGE_CHILDREN, &usage);

        double wait_time = (exec_start_time.tv_sec - program_start_time.tv_sec) + 
                           (exec_start_time.tv_nsec - program_start_time.tv_nsec) / 1e9;
        double burst_time = (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) + 
                            (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1e6;
        double turnaround_time = (exec_end_time.tv_sec - program_start_time.tv_sec) + 
                                 (exec_end_time.tv_nsec - program_start_time.tv_nsec) / 1e9;

        printf("Process %s (PID: %d) completed.\n", processes[winner_index].name, pid);
        printf("Burst Time: %.3f s\n", burst_time);
        printf("Wait Time: %.3f s\n", wait_time);
        printf("Turnaround Time: %.3f s\n", turnaround_time);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

int main() {
    struct process_info processes[MAX_PROCESSES] = {
        {"./cpu1", 0},
        {"./cpu2", 0},
        {"./memory1", 0},
        {"./memory2", 0},
    };

    assign_tickets(processes, MAX_PROCESSES);
    int winner_index = run_lottery(processes, MAX_PROCESSES);

    struct timespec program_start_time;
    clock_gettime(CLOCK_MONOTONIC, &program_start_time); // Get program start time

    if (winner_index != -1) {
        execute_process(winner_index, processes, program_start_time);
    } else {
        printf("Failed to select a winning process.\n");
    }

    return 0;
}
