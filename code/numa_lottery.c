#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <numa.h>

struct process_info {
    char name;
    int burst_time;
    int priority;
    int numa_node; // Preferred NUMA node
    int tickets; // Number of lottery tickets
};

void assign_tickets_based_on_numa(struct process_info *processes, int n) {
    for (int i = 0; i < n; i++) {
        processes[i].tickets = processes[i].burst_time / 10 + 10; 
        if (numa_available() >= 0) {
            int preferred_node = numa_preferred();
            if (processes[i].numa_node == preferred_node) {
                processes[i].tickets += 5; // Extra tickets for matching NUMA node
            }
        }
    }
}

void run_lottery(struct process_info *processes, int n, int total_tickets) {
    srand(time(NULL)); // Seed for randomness
    int winning_ticket = rand() % total_tickets + 1;
    int ticket_count = 0;
    for (int i = 0; i < n; i++) {
        ticket_count += processes[i].tickets;
        if (ticket_count >= winning_ticket) {
            printf("Process %c wins the lottery\n", processes[i].name);
            // Simulate process execution
            break;
        }
    }
}

int main() {
    struct process_info processes[5] = {
        {'A', 100, 1, 0, 0},
        {'B', 150, 1, 1, 0},
        {'C', 200, 1, 0, 0},
        {'D', 250, 1, 1, 0},
        {'E', 300, 1, 0, 0}
    };
    int n = 5;
    int total_tickets = 0;

    if (numa_available() < 0) {
        printf("NUMA is not available on this system.\n");
        return -1;
    }

    assign_tickets_based_on_numa(processes, n);
    for (int i = 0; i < n; i++) {
        total_tickets += processes[i].tickets;
    }

    run_lottery(processes, n, total_tickets);

    return 0;
}
