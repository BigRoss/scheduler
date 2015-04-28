#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define MAXARGS 10

struct pcb {
    pid_t pid;             // system process ID
    char * args[MAXARGS];  // program name and args
    int arrivalTime;
    int remainingCpuTime;
    struct pcb * next;     // links for Pcb handlers
};
typedef struct pcb Pcb;
typedef Pcb * PcbPtr;

int filled;

//Create the queue with size queueSize, return the head of the queue
PcbPtr queueCreate(PcbPtr head);

//Return 1 if success, -1 on error
int enqueue(PcbPtr queueHead, PcbPtr newPcb);

//Pop off the first element in FCFS manner
PcbPtr dequeue(PcbPtr queueHead);

//Return number of Pcbs in the queue, TODO: CHANGE FILLED, will have different filled for each queue
int getFilled();
