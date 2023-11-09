# Priority-and-affinity-in-OS
The core is assigned based on core number from input file, if the input file contains -1, then a free core is assigned.

for the priority, if we assign 3 process to a single core, the high priority process gets more cpu resource , and low priority process gets less cpu, but the processes  assigned to a single core runs in parallel. And the processes attached to a free core used 100% CPU.
