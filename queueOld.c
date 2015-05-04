#include "queue.h"

PcbPtr queueCreate(PcbPtr head){
	PcbPtr queueHead = NULL;
	if(head != NULL){
		queueHead = head;
		filled = 1;
	}
	return queueHead;
}

//Return 1 if success, -1 on error
int enqueue(PcbPtr queueHead, PcbPtr newPcb){
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
		return -1;
	}
	return 1;
}

//Pop off the first element in FCFS manner
PcbPtr dequeue(PcbPtr queueHead){
	if(filled == 0){
		return NULL;
	}
	PcbPtr tmp = queueHead;
	//TODO: free the dequeued element?
	queueHead = tmp->next;
	filled--;
	return tmp;
}

//Return number of Pcbs in the queue
int getFilled(){
	return filled;
}