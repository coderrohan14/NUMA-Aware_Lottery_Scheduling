# Lottery Scheduling incorporating NUMA awareness policies.

## Collaborators

1. Sanath Radhakrishna Pai
2. Rohan Neeraj Pradhan


## Experiment Idea
The goal here is to implement lottery scheduling in general by considering the NUMA awareness policiies discussed in the decade of wasted cores inorder to increase the efficiency.Our Idea is to develop a scheduling algorithm that dynamically adjusts ticket allocations based on processes' NUMA node affinity and memory access patterns, aiming to mitigate the inefficiencies of parallel computing in NUMA systems. This initiative seeks to directly address the performance bottlenecks identified in the paper, by implementing a practical solution that optimizes process scheduling for improved system performance in NUMA architectures.

## Experiment Setup
Since the host operating system for our experiment was Windows with 8GB Ram and 512GB SSD without NUMA support, we needed a type-2 hypervisor set up which allows us to configure hardware resources and set up NUMA. Since oracle virtual box does not allow NUMA configuration, We set up QEMU command line to configure our virtualization environment. However despite having the right configuration, the performance was seriously impacted to the point of timeouts and Hung states. Post this we switched to VMware workstation player and set up a linux vm with 8 CPU processor cores and 6GB Ram and 40 GB storage all of which far exceed the requirements of the linux installation.

Once workstation was set up, we powered off the vm. We made changes the vm configuration file (*.vmx) by adding two lines to the code.
numa.nodes = "4"
numa.vcpu.maxPerVirtualNode = "4"

Post this the vm failed to power on due to conflicting virtualization issues. Hence I disabled the vcpu hot add feature to false on the vmx file and also enabled Intel VT-x support on my host machine by making changes in the BIOS configuration.Post this, the vm powered on successfully.

Inorder to check the NUMA configuration from linux, we installed numactl on linux and checked the hardware configuration to ensure NUMA was configured properly.

## Code

We started with implementing a basic lottery scheduling algorithm for process execution. The main idea behind lottery scheduling is to allocate a certain number of lottery tickets to each process based on its priority or other factors, and then randomly select a winning ticket to determine the next process to be scheduled for execution.

Initially, we implemented a simple version of lottery scheduling without considering any additional factors. In this version, we created a struct process_info to store information about each process, including its process ID (PID), name, burst time (simulated), and the number of lottery tickets assigned to it.

The code included the following functions:

assign_tickets: This function assigned lottery tickets to each process based on its burst time. The formula used was tickets = burst_time / 10 + 10.
run_lottery: This function implemented the lottery scheduling algorithm. It generated a random winning ticket number and iterated through the processes, summing up their tickets. The process whose cumulative ticket count exceeded the winning ticket number was declared the winner.
main: This function created real child processes by forking and executing various commands (like ls, sleep, grep, and tar). It then called the assign_tickets and run_lottery functions to perform lottery scheduling and determine the winning process.
After implementing the basic lottery scheduling algorithm, we extended the code to incorporate Non-Uniform Memory Access (NUMA) awareness. NUMA is a computer memory design used in modern multiprocessor systems, where the memory access time depends on the memory location relative to the processor.

To incorporate NUMA awareness, we made the following changes:

We added a new field numa_node to the process_info struct, which represents the preferred NUMA node for each process.
We modified the assign_tickets function to assign_tickets_based_on_numa. In this function, we first assigned tickets based on the burst time, as before. Then, if NUMA was available on the system, we checked if the process's preferred NUMA node matched the system's preferred NUMA node. If they matched, we awarded the process with 5 extra tickets.
The rationale behind awarding extra tickets to processes with matching NUMA node preferences is to prioritize their execution, as they are likely to experience better performance due to reduced memory access latency.

The rest of the code remained largely the same, with the run_lottery function still implementing the lottery scheduling algorithm to select the winning process.

In summary, we started with a basic implementation of lottery scheduling, which involved assigning tickets to processes based on their burst times and randomly selecting a winner. We then extended this implementation to incorporate NUMA awareness by considering the preferred NUMA node for each process and awarding extra tickets to processes with matching NUMA node preferences. This NUMA-aware lottery scheduling aims to improve overall system performance by prioritizing the execution of processes that are likely to experience lower memory access latency.
## Evaluation

We built a cfs scheduler and computed the burst time and turn around time and compared them with our dynamic numa lottery schedulers and generating graphs using python matplotlib. based on that we drew conclusions that our code shows better results in comparison with CFS and also with normal lotterry scheduler

T

