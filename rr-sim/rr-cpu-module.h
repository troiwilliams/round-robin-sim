#ifndef RR_CPU_MODULE_H_
#define RR_CPU_MODULE_H_

#include "rr-pcb-module.h"

class CPU_Module {
private:
    float cpu_time;             // holds the (current) CPU time
    float time_quantum;         // holds the Round Robin time quantum
    PCB_Module* curr_process;   // points to the current (running) process
    PCB_Module* saved_process;  // points to the saved (not-running) process
    
    // Allows Scheduler_Module class to manipulate this class
    friend class Scheduler_Module;

public:
    // CONSTRUCTOR - CPU MODULE
    CPU_Module(float tq = 20.0f) {
        cpu_time = 0.0f;
        time_quantum = tq;
        curr_process = NULL;
    }

    // DE-CONSTRUCTOR - CPU MODULE
    ~CPU_Module() { /* no implementation */ }

    // GET CPU TIME
    // Returns the current CPU (clock) time
    float GetCpuTime() {
        return cpu_time;
    }

    // EXECUTE PROCESS
    // "Executes" the current process and returns the clock time after process execution
    float Execute() {
        if (curr_process == NULL)
            return -1;
        // 1. get the service time remaining for a process
        float time_left = curr_process->GetTimeRemaining();
        // 2.A. the time remaining is greater than or equal to the time quantum
        if (time_left >= time_quantum) {
            cpu_time += time_quantum;
            curr_process->Fire (time_quantum);
        // 2.B. the time remaining is less than the time quantum
        } else {
            cpu_time += time_left;
            curr_process->Fire (time_left);
        }
        return cpu_time;
    }
    
    // FAST FORWARD CLOCK TO
    // Advances the CPU clock a new clock time (greater than the current one)
    float FastForwardClockTo(float newCpuTime) {
        if (newCpuTime > cpu_time)
            cpu_time = newCpuTime;
        return cpu_time;
    }
    
    // LOAD PROCESS
    // Loads the saved process and makes it the current process
    void LoadProcess() {
        if (saved_process != NULL) {
            curr_process = saved_process;
            saved_process = NULL;
        }
    }
    
    // SAVE PROCESS
    // Saves the current process (using the saved_process variable)
    void SaveProcess() {
        if (curr_process != NULL) {
            saved_process = curr_process;
            curr_process = NULL;
        }
    }
    
    // INTERRUPT
    // Simulates a CPU interrupt and uses "process" to handle the interrupt (scheduler)
    float Interrupt(PCB_Module* process) {
        // 1. save the current process
        SaveProcess();
        // 2. make the current process the new process "process" (scheduler)
        curr_process = process;
        // 3. execute the current process
        Execute();
        curr_process = NULL;
        // 4. load the saved process (make it the current process)
        LoadProcess();
        return cpu_time;
    }
    
    // GET TIME QUANTUM
    // Returns the (round robin) time quantum
    float GetTimeQuantum() {
        return time_quantum;
    }
    
    // BUSY
    // Returns the CPU's current state (if it is currently occupied by a process or not)
    bool Busy() {
        return (curr_process != NULL);
    }
    
    // INTERRUPTED
    // Returns the CPU's interrupted state (checks if a process was saved)
    bool Interrupted() {
        return (saved_process != NULL);
    }
};

#endif // RR_CPU_MODULE_H_
