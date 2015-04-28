#include "queue.h"

#define MAX_BUFFER 1024

PcbPtr createPcb(int pid, int arrivalTime, int priority, int cpuTime, int memAlloc, int res1, int res2 , int res3, int res4);

int main(int argc, int* argv[]){
	char input[MAX_BUFFER];
	int i = 0;
	PcbPtr head;
	while(fgets(input, MAX_BUFFER, stdin) != NULL){
		//Parse the input and save the variables
		int arrivalTime, priority, cpuTime, memAlloc, res1, res2, res3, res4;
		sscanf(input, " %d, %d, %d, %d, %d, %d, %d, %d ", &arrivalTime, &priority, &cpuTime, &memAlloc, &res1, &res2, &res3, &res4);
		//Pid is i + 1
		PcbPtr newPcb = createPcb( (i + 1), arrivalTime, priority, cpuTime, memAlloc, res1, res2, res3, res4);
		if(i == 0){
			head = queueCreate(newPcb);
		}
		else{
			enqueue(head, newPcb);
		}
	}
}

PcbPtr createPcb(int pid, int arrivalTime, int priority, int cpuTime, int memAlloc, int res1, int res2 , int res3, int res4){
	//TODO if success, return? if fail return?
	PcbPtr newPcb = (PcbPtr) malloc(sizeof(Pcb));
	newPcb->pid = pid;
	newPcb->arrivalTime = arrivalTime;
	newPcb->remainingCpuTime = cpuTime;
	newPcb->next = NULL;
	return newPcb;
}

// PcbPtr startPcb(PcbPtr process)
// /*- start a process
// returns:
//   PcbPtr of process
//   NULL if start failed*/

// PcbPtr terminatePcb(PcbPtr process)
// - terminate a process
// returns:
//   PcbPtr of process
//   NULL if terminate failed

// PcbPtr createnullPcb(void)
// /*- create inactive Pcb.
// returns:
//   PcbPtr of newly initialised Pcb
//   NULL if malloc failed*/




