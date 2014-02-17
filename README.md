Round Robin Simulator
=====================

An event-driven simulator that emulates the round robin algorithm for operating system process scheduling. The round robin scheduling algorithm operates by executing each process in a queue for a fixed amount of time, a time quantum. If the process finished executing at or before the time quantum has reached, the process terminates. If the process does not finish, however, it is added to the end of the queue. And, after either case, the next process in the queue is executed in the same fashion.

In this program, a collection of processes are read from a file (in the input directory) and "executed" in each simulation, the complete execution of all processes in the queue. A total of 24 simulations are ran each time the program runs. During each execution of the program, different combinations of time quanta and scheduler overhead (the amount of time it takes to switch from one process to another) are tested. The results from each simulation is outputted to files within the output directory.
