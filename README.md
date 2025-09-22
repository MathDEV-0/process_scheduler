# Process Scheduler

This project is a **process scheduler** written in C.  
It implements the main classical scheduling algorithms used in operating systems, simulating multiple processors, blocking, and quantum management.

---

## Algorithms Implemented

- **FIFO (First-In, First-Out)** → The first process to arrive is the first to be executed.  
- **SJF (Shortest Job First)** → Chooses the process with the shortest remaining execution time (non-preemptive).  
- **SRT (Shortest Remaining Time)** → Preemptive version of SJF, where processes can be interrupted if a shorter one arrives.  
- **RR (Round Robin)** → Each process receives a fixed quantum, and when it expires, it returns to the ready queue.  

---

## Compilation

Compile the simulator with:

gcc -std=c11 -Wall -O2 -o simso main.c pcb.c escalonador.c


## Execution

./simso "<'input'>" <'algorithm'>

#### Base input:

<memory_size> <quantum> | <N_processadores> | <N_processos> | <t0> <t_exec> [b<tempo_bloqueio>] | <t1> <t_exec> [b<tempo_bloqueio>] | ...

#### Concrete example:

./simso "2 5|1|3|0 5 1b3|0 5 4b10|0 6" srt

## Obs:

If no algorithm is specified, the default is Round Robin (RR).

Log format:

time:P<id> -> <'state'> ('<remaining_time>') [<'CPU'>]
