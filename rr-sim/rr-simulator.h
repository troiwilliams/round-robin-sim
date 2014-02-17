#ifndef RR_SIMULATOR_H_
#define RR_SIMULATOR_H_

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

#include "rr-queue-module.h"
#include "rr-pcb-module.h"
#include "rr-scheduler-module.h"
#include "rr-cpu-module.h"

// ==== GLOBAL VARIABLES ============================================================== //
const int MAX_QUEUES = 3;
const int ARRIVALS = 0;
const int READY = 1;
const int DONE = 2;

// ==== FUNCTION PROTOTYPES =========================================================== //
void readfile(Queue_Module&, const char*);
void run_simulator(Queue_Module[], Scheduler_Module*, CPU_Module&);
void perform_analysis(Queue_Module[], CPU_Module, Scheduler_Module*, int, const int);

#endif // RR_SIMULATOR_H_