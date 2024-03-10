// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <numa.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <sys/resource.h>
// #include <sys/mman.h>
// #include <fcntl.h>
// #include <signal.h>

// #define MAX_PROCESSES 10

// struct process_info {
//     pid_t pid;
//     char name[32];
//     int burst_time; // This will be calculated dynamically
//     int priority;
//     int numa_node; // Preferred NUMA node
//     int tickets; // Number of lottery tickets
//     time_t start_time; // Start time for turnaround time calculation
//     struct rusage ru; // Resource usage information
// };

// void bind_process_to_numa_node(pid_t pid, int numa_node) {
//     struct bitmask *mask = numa_allocate_nodemask();
//     numa_bitmask_clearall(mask);
//     numa_bitmask_setbit(mask, numa_node);
//     numa_run_on_node_mask_all(mask); // Bind the current process to the specified NUMA node
//     numa_free_nodemask(mask);
// }

// void assign_tickets_based_on_numa(struct process_info *processes, int n) {
//     for (int i = 0; i < n; i++) {
//         processes[i].tickets = 10; // Initialize with a base number of tickets

//         if (numa_available() >= 0) {
//             int preferred_node = numa_max_node() / 2; // Assign preferred node based on process index
//             processes[i].numa_node = preferred_node;
//             bind_process_to_numa_node(processes[i].pid, preferred_node);

//             if (processes[i].numa_node == numa_preferred()) {
//                 processes[i].tickets += 5; // Extra tickets for matching NUMA node
//             }
//         }
//         // Adjust ticket count based on burst time
//         processes[i].tickets += processes[i].burst_time / 10;
//     }
// }

// void run_lottery(struct process_info *processes, int n, int total_tickets, pid_t *winner_pid) {
//     srand(time(NULL)); // Seed for randomness
//     int winning_ticket = rand() % total_tickets + 1;
//     int ticket_count = 0;

//     for (int i = 0; i < n; i++) {
//         ticket_count += processes[i].tickets;
//         if (ticket_count >= winning_ticket) {
//             printf("Process %s (PID: %d) wins the lottery\n", processes[i].name, processes[i].pid);
//             *winner_pid = processes[i].pid;
//             break;
//         }
//     }
// }

// int main() {
//     struct process_info processes[MAX_PROCESSES];
//     int n = 0;
//     int total_tickets = 0;

//     if (numa_available() < 0) {
//         printf("NUMA is not available on this system.\n");
//         return -1;
//     }

//     // Get the number of NUMA nodes
//     int num_nodes = numa_num_configured_nodes();
//     printf("Number of NUMA nodes: %d\n", num_nodes);

//     // Create real processes
//     char *commands[] = {"./cpu1", "./cpu2", "./memory1", "./memory2"};
//     int num_commands = sizeof(commands) / sizeof(char *);

//     for (int i = 0; i < num_commands; i++) {
//         pid_t pid = fork();
//         if (pid == 0) { // Child process
//             execl("/bin/sh", "sh", "-c", commands[i], NULL);
//             _exit(0); // If exec fails
//         } else if (pid > 0) { // Parent process
//             processes[n].pid = pid;
//             snprintf(processes[n].name, sizeof(processes[n].name), "%s", commands[i]);
//             processes[n].start_time = time(NULL); // Record start time
//             processes[n].numa_node = -1; // Preferred NUMA node not set yet
//             n++;
//         } else {
//             perror("fork");
//             return -1;
//         }
//     }

//     // Wait for child processes to finish and calculate burst time, turnaround time, and resource usage
//     for (int i = 0; i < n; i++) {
//         int status;
//         pid_t pid = waitpid(processes[i].pid, &status, 0);
//         if (pid > 0 && WIFEXITED(status)) {
//             getrusage(RUSAGE_CHILDREN, &processes[i].ru);
//             processes[i].burst_time = processes[i].ru.ru_utime.tv_sec * 1000 + processes[i].ru.ru_utime.tv_usec / 1000 + processes[i].ru.ru_stime.tv_sec * 1000 + processes[i].ru.ru_stime.tv_usec / 1000; // Convert to milliseconds
//             time_t end_time = time(NULL);
//             time_t turnaround_time = end_time - processes[i].start_time;
//             printf("Process %s (PID: %d) completed. Burst Time: %d ms, Turnaround Time: %ld s\n", processes[i].name, processes[i].pid, processes[i].burst_time, turnaround_time);
//         } else {
//             perror("waitpid");
//             return -1;
//         }
//     }

//     assign_tickets_based_on_numa(processes, n);

//     for (int i = 0; i < n; i++) {
//         total_tickets += processes[i].tickets;
//     }

//     // Create shared memory segment for winner's process ID
//     int shm_fd = shm_open("/winner_pid", O_CREAT | O_RDWR, 0666);
//     if (shm_fd == -1) {
//         perror("shm_open");
//         return -1;
//     }

//     ftruncate(shm_fd, sizeof(pid_t));
//     pid_t *winner_pid = mmap(NULL, sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
//     if (winner_pid == MAP_FAILED) {
//         perror("mmap");
//         return -1;
//     }

//     run_lottery(processes, n, total_tickets, winner_pid);

//     // Child processes check if they are the winner
//     for (int i = 0; i < n; i++) {
//         if (processes[i].pid != *winner_pid) {
//             printf("Process %s (PID: %d) did not win the lottery, terminating.\n", processes[i].name, processes[i].pid);
//             kill(processes[i].pid, SIGKILL); // Use SIGKILL to forcefully terminate the process
//         } else {
//             printf("Process %s (PID: %d) is the winner, executing main functionality.\n", processes[i].name, processes[i].pid);
//             // Execute the main functionality of the winning process here
//         }
//     }

//     // Clean up shared memory
//     munmap(winner_pid, sizeof(pid_t));
//     shm_unlink("/winner_pid");

//     return 0;
// }

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
    return -1; // Should not happen
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
        // Wait time cannot be directly calculated in this context without more detailed scheduling info

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
