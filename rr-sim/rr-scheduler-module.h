#ifndef RR_SCHEDULER_MODULE_H_
#define RR_SCHEDULER_MODULE_H_

#include <cstdlib>
using namespace std;

#include "rr-pcb-module.h"
#include "rr-queue-module.h"
#include "rr-cpu-module.h"

class Scheduler_Module : public PCB_Module {
protected:
    // the scheduler needs to know of all the queues (dealing with scheduling) and the
    // CPU in order to dispatch jobs on it
    Queue_Module* readyQ;       // points to the ready queue
    Queue_Module* arrivalsQ;    // points to the arrivals queue
    Queue_Module* doneQ;        // points to the done queue
    CPU_Module* cpu;            // points to the CPU
    int readyQ_length_sum;      // sum of the length of the ready queue during dispatch
    int timesChecked;           // amount of times ready queue was checked
    
public:
    // CONSTRUCTOR - SCHEDULER MODULE
    Scheduler_Module(Queue_Module* ready_queue, Queue_Module* arrivals_queue,
                        Queue_Module* done_queue, CPU_Module* processor,
                        float overhead = 0.0f) : PCB_Module (9032, 0.0f, overhead)
    {
        if (ready_queue == NULL || arrivals_queue == NULL || 
            arrivals_queue == NULL || done_queue == NULL || processor == NULL) {
            cerr << "Something was NULL\n";
            exit (1);
        }
        readyQ = ready_queue;
        arrivalsQ = arrivals_queue;
        doneQ = done_queue;
        cpu = processor;
        ChangeState(PCB_Module::PS_READY, cpu->cpu_time);
        readyQ_length_sum = 0;
        timesChecked = 0;
    }
    
    // DE-CONSTRUCTOR - SCHEDULER MODULE
    ~Scheduler_Module () {
        readyQ = NULL;
        arrivalsQ = NULL;
        doneQ = NULL;
        cpu = NULL;
    }
    
    // DISPATCH NEXT PROCESS
    // Looks in the ready queue and pops off the next process to execute
    void DispatchNextProcess() {
        // 1. check if the ready queue is empty
        // 1.A. if it is not:
        if (!readyQ->Empty()) {
            // add the length of the ready queue to the sum (avg length calculation)
            readyQ_length_sum += readyQ->Length();
            // increment (by one) the amount of times checked (avg length calculation)
            timesChecked++;
            // dequeue a process and save it to the CPU (it will be loaded automatically)
            cpu->saved_process = readyQ->Dequeue();
            // change the process's state to running and log the CPU time
            cpu->saved_process->ChangeState(PCB_Module::PS_RUNNING, cpu->cpu_time);
        // 1.B. if it is:
        } else {
            cpu->saved_process = NULL;
        }
    }
    
    // EVALUATE SAVED PROCESS
    // Evaluates the saved process (figures out what to do with it)
    void EvaluateSavedProcess() {
        // if the CPU is interrupted
        if (cpu->Interrupted()) {
            // get current cpu time
            float cputime = cpu->cpu_time;
            // if the process has time remaining, add it back to the ready queue
            if (cpu->saved_process->GetTimeRemaining() > 0.0f) {
                cpu->saved_process->ChangeState(PCB_Module::PS_READY, cputime);
                readyQ->Enqueue(cpu->saved_process);
            // else it has finished execution, add it to the done queue
            } else {
                cpu->saved_process->ChangeState(PCB_Module::PS_TERMINATED, cputime);
                doneQ->Enqueue(cpu->saved_process);
                // if the simulation is done, subtract the scheduler overhead from the
                // the cpu time because the overhead is added at the end making the cpu
                // time longer than it should be (1 overhead longer)
                if (doneQ->Length() == arrivalsQ->MaxLength())
                    cpu->cpu_time -= GetOverhead();
            }
            cpu->saved_process = NULL;
        }
    }
    
    // CHECK FOR NEW PROCESSES
    // Looks in the arrivals queue for processes whose arrival times are 
    // less than or equal to the current CPU time.
    int CheckForNewProcesses() {
        int size = readyQ->Length(), amountFound = 0;
        float cputime = cpu->cpu_time;
        while (!arrivalsQ->Empty() && arrivalsQ->Front().GetArrivalTime() <= cputime) {
            PCB_Module* tmp = arrivalsQ->Dequeue();
            tmp->ChangeState(PCB_Module::PS_READY, cputime);
            readyQ->Enqueue(tmp);
            tmp = NULL;
        }
        amountFound = readyQ->Length() - size;
        return amountFound;
    }
    
    // FIRE
    // Executes the scheduler, searches for additional processes that have "recently
    // been submitted" (while the saved process was executing), and performs a
    // context switch (swapping the saved process for another process in the ready queue)
    virtual float Fire(float amount = 0.0f) {
        // 1. check for new arrival processes
        int processesAdded = CheckForNewProcesses();
        // 2. perform a context switch
        ContextSwitch();
        return timeLeft;
    }
    
    // CONTEXT SWITCH
    // Switches execution from the previous process to the new process
    void ContextSwitch() {
        EvaluateSavedProcess();
        DispatchNextProcess();
    }
    
    // GET OVERHEAD
    // Returns the scheduler overhead
    float GetOverhead() {
        return GetServiceTime();
    }
    
    // GET AVG READY QUEUE LENGTH
    // Returns the average length of the ready queue
    float GetAvgReadyQueueLength() {
        return ((float)readyQ_length_sum / timesChecked);
    }
};

#endif // RR_SCHEDULER_MODULE_H_
