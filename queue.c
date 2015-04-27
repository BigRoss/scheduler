#include "queue.h"

void queueCreate(PcbPtr head){
	if(head != NULL){
		queueHead = head;
	}
	else{
		return;
	}
	filled = 1;
}

//Return 1 if success, -1 on error
int enqueue(PcbPtr newPcb){
	//New entered Pcb's should have a next pointer which is = NULL
	if(newPcb != NULL){
		//Loop through linked list until last element then add newPcb to the end
		PcbPtr currPcb = queueHead;
		while(currPcb->next != NULL){
			currPcb = currPcb->next;
		}
		currPcb->next = newPcb;
		newPcb->next = NULL;
		filled++;
	}
	else{
		return NULL;
	}
}

//Pop off the first element in FCFS manner
PcbPtr dequeue(){
	PcbPtr tmp = queueHead;
	queueHead = tmp->next;
	filled--;
	return tmp;
}

//Look at the top element in FCFS manner
PcbPtr peek(){
	PcbPtr ret = queueHead;
	return ret;
}

//Return number of Pcbs in the queue
int getFilled(){
	return filled;
}