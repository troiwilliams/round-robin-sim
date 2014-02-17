#ifndef RR_PCB_MODULE_H_
#define RR_PCB_MODULE_H_

#include <string>
#include <cstdlib>
#include <iostream>
using namespace std;

class PCB_Module {
public:
    // Describes a process's state
    enum ProcessState_t {
        PS_NULL = 0,        // before the process has been "submitted"
        PS_READY,           // when in the ready queue
        PS_RUNNING,         // when running on the cpu
        PS_TERMINATED       // when finished running and time left = 0
    };
    friend class Queue_Module;
    
protected:
    unsigned int processId; // process ID
    float arrivalTime;      // arrival time [CPU clock]
    float serviceTime;      // amount of time this process will need to execute
    float timeLeft;         // time left after one or more executions
    float startTime;        // time this process started executing [CPU clock]
    float endTime;          // time this process terminated (exited system) [CPU clock]
    ProcessState_t state;   // state of the process
    PCB_Module* next;       // used to point to the next process on the queue
    
public:
    // CONSTRUCTOR - PCB MODULE
    PCB_Module(unsigned int ID, float arrTime = 0.0f, float servTime = 0.0f) {
        processId = ID;
        arrivalTime = arrTime;
        serviceTime = servTime;
        timeLeft = serviceTime;
        state = PS_NULL;
        startTime = 0.0f;
        endTime = 0.0f;
    }
    
    // DE-CONSTRUCTOR - PCB MODULE
    ~PCB_Module() { /* no implementation */ }
    
    // GET PROCESS ID
    // Returns the process's ID
    unsigned int GetProcessId() {
        return processId;
    }
    
    // GET ARRIVAL TIME
    // Returns the process's arrival time
    float GetArrivalTime() {
        return arrivalTime;
    }
    
    // GET TIME REMAINING
    // Returns the amount of time remaining to service
    float GetTimeRemaining() {
        return timeLeft;
    }
    
    // GET SERVICE TIME
    // Returns the amount of time needed to service the process
    float GetServiceTime() {
        return serviceTime;
    }
    
    // BEGAN EXECUTION
    // Returns the (cpu) time this process began executing
    float BeganExecution() {
        return startTime;
    }
    
    // COMPLETED EXECUTION
    // Returns the (cpu) time this process completed
    float CompletedExecution() {
        return endTime;
    }
    
    // FIRE
    // Decreases the time left by amount and returns the time left
    virtual float Fire(float amount = 0.0f) {
        timeLeft -= amount;
        return timeLeft;
    }
    
    // GET PROCESS STATE
    // Returns the process's current state
    ProcessState_t GetProcessState() {
        return state;
    }
    
    // PROCESS STATE TO STRING
    // Returns the process's state in string form
    string ProcessState_To_String() {
        switch (state) {
            case PCB_Module::PS_READY:
                return "READY";
                
            case PCB_Module::PS_RUNNING:
                return "RUNNING";
                
            case PCB_Module::PS_TERMINATED:
                return "TERMINATED";
            
            case PCB_Module::PS_NULL:   
            default:
                return "NULL";
        }
    }
    
    // CHANGE STATE
    // Changes the process's state to the newly specified state
    void ChangeState(PCB_Module::ProcessState_t newState, float cpuTime) {
        state = newState;
        // if it is the first time the process is executing, log the CPU time
        // (used for waiting time)
        if (FirstTimeExecuting())
            startTime = cpuTime;
        // if the process is terminated, log the CPU time (used for turn around time)
        if (state == PCB_Module::PS_TERMINATED)
            endTime = cpuTime;
    }
    
    // FIRST TIME EXECUTING
    // Checks if this is the first time this process is executing
    bool FirstTimeExecuting() {
        return (state == PCB_Module::PS_RUNNING && timeLeft == serviceTime);
    }
};

#endif // RR_PCB_MODULE_H_
