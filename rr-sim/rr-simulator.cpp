#include "rr-simulator.h"

ofstream excel_out;
ofstream analysis_out;

int main(int argc, char** argv) {
    const char* infile;
    if (argc > 2) {
        cerr << "Too many arguments... Continuing.\n";
        infile = argv[1];
    } else if (argc == 2) {
        infile = argv[1];
    } else {
        infile = "../input/process-list.txt";
    }
    // scheduler overhead and cpu time quantum during a given experiment
    float schedulerOverhead = 0.0f;
    float cpuTimeQuantum = 0.0f;
    
    // list of overheads and time quantum (in seconds)
    const int NUMB_OF_OVERHEADS = 6;
    const int NUMB_OF_TQS = 4;
    const int NUMB_OF_RUNS = NUMB_OF_OVERHEADS + NUMB_OF_TQS;
    float overheads[NUMB_OF_OVERHEADS] = { 0.0f, 0.005f, 0.01f, 0.015f, 0.02f, 0.025f };
    float time_quantums[NUMB_OF_TQS] = { 0.05f, 0.1f, 0.25f, 0.5f };
    int count = 1;
    
    // open output files
    excel_out.open("../output/excel-output.txt");
    analysis_out.open("../output/analysis-output.txt");
    // check if the output files are open
    if (!excel_out.good() || !analysis_out.good()) {
        cerr << "One of the output files cannot open!\nExiting...\n";
        exit (EXIT_FAILURE);
    }
    // performs all of the experiments by running two for loops (a regular and a nested
    // for loop)
    for (int tq_i = 0; tq_i < NUMB_OF_TQS; tq_i++) {
        cpuTimeQuantum = time_quantums[tq_i];
        for (int overhead_i = 0; overhead_i < NUMB_OF_OVERHEADS; overhead_i++) {
            analysis_out << "START SIMULATION #" << count << "\n";
            schedulerOverhead = overheads[overhead_i];
            Scheduler_Module* scheduler;
            Queue_Module queue[MAX_QUEUES];
            CPU_Module cpu(cpuTimeQuantum);
            scheduler = new Scheduler_Module(&queue[READY], &queue[ARRIVALS],
                                                &queue[DONE], &cpu, schedulerOverhead);
            // ==== initialization ==== //
            readfile(queue[ARRIVALS], infile);
            // ==== simulation ==== //
            run_simulator(queue, scheduler, cpu);
            // ==== do analysis ==== //
            perform_analysis(queue, cpu, scheduler, count, NUMB_OF_RUNS);
            delete scheduler;
            analysis_out << "END SIMULATION [#" << count << "]\n\n\n";
            count++;
        }
    }
    excel_out.close();
    analysis_out.close();
    return 0;
}

// READ FILE
// Reads the process input file with each processes arrival and service times
void readfile(Queue_Module& queue, const char* fileStr) {
    if (fileStr == NULL) {
        cerr << "The input file absolute location was no specified. Exiting...\n\n";
        exit (EXIT_FAILURE);
    }
    ifstream ifs(const_cast<char *>(fileStr));
    if (ifs.is_open()) {
        string line;
        const char* WHITESPACE = " \t\n";
        int id = 0;
        while (ifs.good()) {
            getline(ifs, line);
            char* arrival_cstr = strtok(const_cast<char *>(line.c_str()), WHITESPACE);
            char* service_cstr = strtok(NULL, WHITESPACE);
            float arrtime = atof(arrival_cstr);
            float servtime = atof(service_cstr);
            queue.Enqueue(new PCB_Module (id++, arrtime, servtime));
        }
        ifs.close();
    } else {
        cerr << "Could not open input file. Exiting...\n\n";
        exit(EXIT_FAILURE);
    }
}

// RUN SIMULATOR
// This function acts as the Round Robin Simulator
void run_simulator(Queue_Module queue[], Scheduler_Module* scheduler, CPU_Module& cpu) {
    float cpu_time = cpu.GetCpuTime();
    const int totalProcessCount = queue[ARRIVALS].Length();
    // check if the ready and arrivals queues are empty and the cpu's not busy
        // [yes] break out of loop
        // [ no] continue with execution loop
    while (queue[DONE].Length() < totalProcessCount) {
        // I. check if the ready queue is empty and the cpu's status
        if (queue[READY].Empty() && !cpu.Busy()) {
            // I.1. get the first process from the arrivals queue
            PCB_Module tmp = queue[ARRIVALS].Front();
            // I.2. fast fwd the cpu's clock to the new process's arrival time
            cpu_time = cpu.FastForwardClockTo(tmp.GetArrivalTime());
            // I.3. dequeue the arrival process and enqueue it into the ready queue
            PCB_Module* ptmp = queue[ARRIVALS].Dequeue();
            queue[READY].Enqueue(ptmp);
            ptmp = NULL;
            // I.4. dispatch the new process to the cpu
            scheduler->DispatchNextProcess();
            // I.5. load the new process on the cpu
            cpu.LoadProcess();
            // I.4. dequeue it from the ready queue and "place it on the cpu"
        }
        // II. execute the current process
        cpu_time = cpu.Execute();
        // III. interrupt signal [time for scheduling duties then continue execution]
        cpu_time = cpu.Interrupt(scheduler);
    }
}

// PERFORM ANALYSIS
// Calculates the deliverables and outputs them to stdin.
void perform_analysis(Queue_Module queue[], CPU_Module cpu,
                        Scheduler_Module* scheduler, int count, const int MAX)
{
    float avgTurnTime = 0.0f, avgWaitTime = 0.0f;
    const int len = queue[DONE].Length();
    float turnTime = 0.0f, waitTime = 0.0f;
    for (int i = 0; i < len; i++) {
        PCB_Module* tmp = queue[DONE].Dequeue();
        turnTime = tmp->CompletedExecution() - tmp->GetArrivalTime();
        waitTime = tmp->BeganExecution() - tmp->GetArrivalTime();
        avgTurnTime += turnTime;
        avgWaitTime += waitTime;
        delete tmp;
    }
    avgTurnTime /= len;
    avgWaitTime /= len;
    
    if (analysis_out.good()) {
        analysis_out << "SCHEDULER OVERHEAD = " << scheduler->GetOverhead() << " secs\n";
        analysis_out << "CPU ROUND ROBIN TQ = " << cpu.GetTimeQuantum() << " secs\n";
        analysis_out << "Analysis:\n";
        analysis_out << "\tREADY queue max length = " << queue[READY].MaxLength() << endl;
        analysis_out << "\tAvg READY queue length = " 
                    << scheduler->GetAvgReadyQueueLength() << endl;
        analysis_out << "\tTOTAL CPU Sim. Time = " << cpu.GetCpuTime() << " secs\n";
        analysis_out << "\tAverage Turn Around Time = " << avgTurnTime << " secs\n";
        analysis_out << "\tAverage Wait Time = " << avgWaitTime << " secs\n";
    }
    
    if (excel_out.good()) {
        if (count == 1) {
            excel_out << "SCHEDULER OVERHEAD\tCPU ROUND ROBIN TQ"
                        << "\tREADY queue max length\tAvg Ready queue length"
                        << "\tTOTAL CPU Sim. Time\tAverage Turn Around Time"
                        << "\tAverage Wait Time\n";
        }
        excel_out << scheduler->GetOverhead() << "\t" << cpu.GetTimeQuantum() << "\t"
            << queue[READY].MaxLength() << "\t" << scheduler->GetAvgReadyQueueLength() 
            << "\t" << cpu.GetCpuTime() << "\t" << avgTurnTime
            << "\t" << avgWaitTime << endl;
    } else {
        cerr << "Could not open output file. Exiting...\n\n";
        exit(EXIT_FAILURE);
    }
}