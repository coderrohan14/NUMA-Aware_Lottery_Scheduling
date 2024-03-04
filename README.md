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

1. We implemented a basic form of lottery scheduling where each process was a dummy. We implemented the algorithm was lottery scheduling and ensured that it works.
2. The next step was to consider NUMA into the picture. We added a struct with numa node affinity.
3. For the process with a matching numa node, we provided that process with 5 extra tickets.
4. Rest of the process was regular lottery scheduling to decide the winner.

## Next Steps

The next step is to implement lottery scheduling with having actual system calls instead of dummy process and also add some extra features mentioned in the decade of wasted cores paper. Also we need to evaluate the execution of different processes and generate graphs for comparison.


