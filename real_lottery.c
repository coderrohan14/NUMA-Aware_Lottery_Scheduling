#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_PROCESSES 10

struct process_info {
    pid_t pid;
    char name[32];
    int burst_time;
    int priority;
    int tickets; // Number of lottery tickets
};

void assign_tickets(struct process_info *processes, int n) {
    for (int i = 0; i < n; i++) {
        processes[i].tickets = processes[i].burst_time / 10 + 10; // Basic ticket assignment
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

    // Create real processes
    char *commands[] = {"ls -l", "sleep 5", "tar -xf archive.tar.gz"};
    int num_commands = sizeof(commands) / sizeof(char *);

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            execl("/bin/sh", "sh", "-c", commands[i], NULL);
            _exit(0); // If exec fails
        } else if (pid > 0) { // Parent process
            processes[n].pid = pid;
            snprintf(processes[n].name, sizeof(processes[n].name), "%s", commands[i]);
            processes[n].burst_time = rand() % 100 + 10; // Simulate burst time
            n++;
        } else {
            // Fork failed
            perror("fork");
            return -1;
        }
    }

    assign_tickets(processes, n);

    for (int i = 0; i < n; i++) {
        total_tickets += processes[i].tickets;
    }

    run_lottery(processes, n, total_tickets);

    // Wait for child processes to finish
    for (int i = 0; i < n; i++) {
        waitpid(processes[i].pid, NULL, 0);
    }

    return 0;
}