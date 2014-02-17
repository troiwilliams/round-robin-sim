#ifndef RR_QUEUE_MODULE_H_
#define RR_QUEUE_MODULE_H_

#include "rr-pcb-module.h"

class Queue_Module {
private:
    PCB_Module* head;       // points to the "head" of the queue
    PCB_Module* tail;       // points to the "tail" of the queue
    unsigned int size;      // states the size of the queue
    unsigned int maxLength; // states the max length of the queue
    
public:
    // CONSTRUCTOR - QUEUE MODULE
    Queue_Module() {
        head = NULL;
        tail = NULL;
        size = 0;
        maxLength = 0;
    }
    
    // DE-CONSTRUCTOR - QUEUE MODULE
    ~Queue_Module() { /* no implementation */ }
    
    // ENQUEUE
    // Receives a "new" process and places it at the end of the queue
    void Enqueue(PCB_Module* process) {
        if (process != NULL) {
            // if the head & tail are not NULL, add the process to the end of the queue
            if (head != NULL && tail != NULL) {
                tail->next = process;
                tail = tail->next;
            // the queue is empty, so make the process the head of the queue
            } else {
                head = process;
                tail = head;
            }
            tail->next = NULL;
            size++;
            // if the size has grown larger than the max length, log the new max length
            if (size > maxLength) {
                maxLength = size;
            }
        }
    }
    
    // DEQUEUE
    // Removes and returns the process at the beginning of the queue
    PCB_Module* Dequeue() {
        if (head == NULL)
            return NULL;
        PCB_Module* process = head;
        head = process->next;
        process->next = NULL;
        size--;
        if (Length() == 0) {
            head = NULL;
            tail = head;
        }   
        return process;
    }
    
    // FRONT
    // Retrieves (a copy of) the process at the front of the queue
    PCB_Module Front() {
        if (Length() <= 0)
            return NULL;
        // creates a copy of the process at the front of the queue
        PCB_Module tmp(head->GetProcessId(), head->GetArrivalTime(),
                        head->GetServiceTime());
        return tmp;
    }
    
    // LENGTH
    // Returns the length/size of the queue
    unsigned int Length() const {
        return size;
    }
    
    // MAX LENGTH
    // Returns the maximum length of the queue
    unsigned int MaxLength() {
        return maxLength;
    }
    
    // EMPTY
    // Returns the queue's capacity status (empty or not)
    bool Empty() const {
        return (size == 0);
    }
    
    // PRINT QUEUE
    // Prints all the process control blocks (PCBs) in the queue
    void PrintQueue() {
        PCB_Module* iter = head;
        cout << endl;
        for (; iter != NULL; iter = iter->next) {
            cout << "PID:       " << iter->GetProcessId() << endl;
            cout << "ArrTime:   " << iter->GetArrivalTime() << endl;
            cout << "SerTime:   " << iter->GetServiceTime() << endl;
            if (iter->BeganExecution() > 0.0f) {
                cout << "StTime:    " << iter->BeganExecution() << endl;
                cout << "EdTime:    " << iter->CompletedExecution() << endl;
            }
            cout << endl;
        }
        iter = NULL;
        cout << endl << endl;
    }
};

#endif // RR_QUEUE_MODULE_H_
