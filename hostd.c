#include "queue.h"

PcbPtr createPcb(int pid, int arrivalTime, int priority, int cpuTime, int memAlloc, int res1, int res2 , int res3, int res4);

int main(int argc, int* argv[]){
	Pcb pcb1;
	pcb1.pid = 1;
	pcb1.next = NULL;
	PcbPtr ptr1 = &pcb1;
	Pcb pcb2;
	pcb2.pid = 2;
	PcbPtr ptr2 = &pcb2;
	pcb2.next = NULL;
	Pcb pcb3;
	pcb3.pid = 3;
	PcbPtr ptr3 = &pcb3;
	pcb3.next = NULL;
	queueCreate(ptr1);
	printf("Filled: %d\n", getFilled());
	enqueue(ptr2);
	printf("Filled: %d\n", getFilled());
	enqueue(ptr3);
	printf("Filled: %d\n", getFilled());
	printf("Pid: %d\n", dequeue()->pid);
	printf("Filled: %d\n", getFilled());
	printf("Pid: %d\n", dequeue()->pid);
	printf("Filled: %d\n", getFilled());
	printf("Pid: %d\n", dequeue()->pid);
	printf("Filled: %d\n", getFilled());
}

PcbPtr createPcb(int pid, int arrivalTime, int priority, int cpuTime, int memAlloc, int res1, int res2 , int res3, int res4){
	Pcb newPcb;
	newPcb.pid = pid;
	newPcb.arrivalTime = arrivalTime;
	newPcb.remaingingCpuTime = CpuTime;
	newPcb.next = NULL;
	PcbPtr ret = &newPcb;
	return ret;
}




