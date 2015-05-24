/*******************************************************************

OS Eercises - Project 2 - HOST dispatcher - Exercise 7

    hostd

        hostd is fcfs 'dispatcher' that reads in a list of 'jobs' from a file
        and 'dispatches' them in a first-come-first-served manner.

        time resolution is one second (although this can be changed). 

    usage
   
        hostd <dispatch file>
 
        where
            <dispatch file> is list of process parameters as specified 
                for assignment 2.

    functionality

    1. Initialize dispatcher queue;
    2. Fill dispatcher queue from dispatch list file;
    3. Start dispatcher timer;
    4. While there's anything in the queue or there is a currently running process:
        i. If a process is currently running;
            a. Decrement process remainingcputime;
            b. If times up:
                A. Send SIGINT to the process to terminate it;
                B. Free up process structure memory
       ii. If no process currently running &&
            dispatcher queue is not empty &&
            arrivaltime of process at head of queue is <= dispatcher timer:
            a. Dequeue process and start it (fork & exec)
            b. Set it as currently running process;
      iii. sleep for one second;
       iv. Increment dispatcher timer;
        v. Go back to 4.
    5. Exit

********************************************************************

version: 1.0 (exercise 7)
date:    December 2003
author:  Dr Ian G Graham, ian.graham@griffith.edu.au

*******************************************************************/

#include "hostd.h"

#define VERSION "1.0"

/******************************************************
 
   internal functions
   
 ******************************************************/

char * StripPath(char*);
void PrintUsage(FILE *, char *);
void SysErrMsg(char *, char *);
void ErrMsg(char *, char *);

/******************************************************/

int main (int argc, char *argv[]) {
    char * inputfile;             // job dispatch file
    int availMem = USER_MEMORY_SIZE;   //How much memory is available
    FILE * inputliststream;
    PcbPtr inputqueue = NULL;     // input queue buffer
    PcbPtr currentprocess = NULL; // current process
    PcbPtr process = NULL;        // working pcb pointer
    PcbPtr rrQueue = NULL;		  // Round robin queue
    PcbPtr fbQueue1 = NULL;       // Highest priority feedback queue
    PcbPtr fbQueue2 = NULL;		  // Middle priority 
    PcbPtr fbQueue3 = NULL;		  // Lowest priority fbQueue
    PcbPtr userQueue = NULL;      // User Queue
    int timer = 0;                // dispatcher timer 
    //Make the MabPtr for the full memory
    MabPtr memory = NULL;
    memory->offset = 0;
    memory->size = availMem;
    memory->allocated = 0;
    memory->next = NULL;
    memory->prev = NULL;

//  0. Parse command line

    if (argc == 2) inputfile = argv[1];
    else PrintUsage (stderr, argv[0]);

//  1. Initialize dispatcher queue;
//     (already initialised in assignments above)
    
//  2. Fill dispatcher queue from dispatch list file;
    
    if (!(inputliststream = fopen(inputfile, "r"))) { // open it
        SysErrMsg("could not open dispatch list file:", inputfile);
        exit(2);
    }

    while (1) {
        int val_check;
        process = createnullPcb();
        if ((val_check = fscanf(inputliststream, "%d, %d, %d, %d, %d, %d, %d, %d",
          &(process->arrivaltime), &(process->priority),
          &(process->remainingcputime), &(process->mbytes),
          &(process->req.printers), &(process->req.scanners),
          &(process->req.modems), &(process->req.cds))) == 8) {
            process->status = PCB_INITIALIZED;
            inputqueue = enqPcb(inputqueue, process);

        }
        else if (val_check >= 0){
            free(process);
            ErrMsg("Invalid input file", inputfile);
            exit(2);
        }
        else if (ferror(inputliststream)) {
            free(process);
            SysErrMsg("Error reading input file", inputfile);
            exit(2);
        }
        else {
            free(process);
            break;
        }
    }
//  3. Start dispatcher timer;
//     (already set to zero above)
    

// 4.     While there's anything in any of the queues or there is a currently running process:
    while(inputqueue != NULL || rrQueue != NULL || currentprocess != NULL || fbQueue1 != NULL || fbQueue2 != NULL || fbQueue3 != NULL || userQueue != NULL){
        //printf("Timer: %d\n", timer);
        //i.Unload pending processes from the input queue: While (head-of-input-queue.arrival-time <= dispatcher timer)
        
    	while(inputqueue!= NULL && inputqueue->arrivaltime <= timer){
        	// Unload pending processes from the input queue: 
            userQueue = enqPcb(userQueue, deqPcb(&inputqueue));
        }  
        
        //  While (head-of-user-job-queue.mbytes can be allocated)
        while(userQueue!= NULL && userQueue->mbytes <= availMem){
            //dequeue process from user job queue, allocate memory to the process and enqueue on highest priority feedback queue 
            //(assigning it the appropriate priority);
            userQueue->priority = 1;
            userQueue->memoryblock = memSplit(memory, userQueue->mbytes);
            memory = memory->next;
            fbQueue1 = enqPcb(fbQueue1, deqPcb(&userQueue));
        }

        // ii.If a process is currently running:
        if(currentprocess != NULL){
            //printf("Decrement time of process: %d\n", currentprocess->pid);
            // a. Decrement process remainingcputime;
            currentprocess->remainingcputime = currentprocess->remainingcputime - QUANTUM;

            // b. If times up:
            if(currentprocess->remainingcputime <= 0){
                //printf("terminate currentprocess as time == 0\n");
                //A. Send SIGINT to the process to terminate it;
                terminatePcb(currentprocess);
                //B.    Free memory allocated to the process;
                memFree(currentprocess->memoryblock);
                //C. Free up process structure memory
                free(currentprocess);
                currentprocess = NULL;
            }
            //c.    else if other processes are waiting in any of the feedback queues:
            else if(fbQueue1 != NULL || fbQueue2 != NULL || fbQueue3 != NULL){
				//A.    Send SIGTSTP to suspend it;
				currentprocess = suspendPcb(currentprocess);

				//B.    Reduce the priority of the process (if possible) and enqueue it on the appropriate feedback queue
				if(currentprocess->priority == 1){
					currentprocess->priority++;
					fbQueue2 = enqPcb(fbQueue2, currentprocess);
				}
				else if(currentprocess->priority == 2){
					currentprocess->priority++;
					fbQueue3 = enqPcb(fbQueue3, currentprocess);
				}
				else{
					fbQueue3 = enqPcb(fbQueue3, currentprocess);
				}
                
                currentprocess = NULL;
            }
        }
    	
        // iii. If no process currently running && RR queue is not empty:
        if(currentprocess == NULL && (fbQueue1 != NULL || fbQueue2 != NULL || fbQueue3 != NULL)){
            //printf("dequeue and start from rrQueue\n");
        	PcbPtr newPcb = NULL;
            //Dequeue a process from the highest priority feedback queue that is not empty
            if(fbQueue1 != NULL){
            	printf("Starting from queue1\n");
            	newPcb = deqPcb(&fbQueue1);
            }
            else if(fbQueue2 != NULL){
            	printf("Starting from queue2\n");
            	newPcb = deqPcb(&fbQueue2);
            }
            else if(fbQueue3 != NULL){
            	printf("Starting from queue3\n");
            	newPcb = deqPcb(&fbQueue3);
            }
            currentprocess = startPcb(newPcb);
        }  
        //     iii. sleep for one second;
        sleep(1);
            
//      iv. Increment dispatcher timer;

        timer = timer + QUANTUM;
            
//       v. Go back to 4.                                        
    }
    
// 5.     Exit.

    exit (0);
}    


/*******************************************************************

char * StripPath(char * pathname);

  strip path from file name

  pathname - file name, with or without leading path

  returns pointer to file name part of pathname
    if NULL or pathname is a directory ending in a '/'
        returns NULL
*******************************************************************/

char * StripPath(char * pathname) {
    char * filename = pathname;\

    if (filename && *filename) {           // non-zero length string
        filename = strrchr(filename, '/'); // look for last '/'
        if (filename)                      // found it
            if (*(++filename))             //  AND file name exists
                return filename;
            else
                return NULL;
        else
            return pathname;               // no '/' but non-zero length string
    }                                      // original must be file name only
    return NULL;
}

/*******************************************************
 * print usage
 ******************************************************/
void PrintUsage(FILE * stream, char * progname) {
    if (!(progname = StripPath(progname))) {
        progname = DEFAULT_NAME;
    }
    
    fprintf(stream,"\n"
      "%s process dispatcher (version " VERSION "); usage:\n\n"
      "  %s <dispatch file>\n"
      " \n"
      "  where <dispatch file> is list of process parameters \n\n",
      progname,progname);

    exit(127);
}
/********************************************************
 * print an error message on stderr
 *******************************************************/

void ErrMsg(char * msg1, char * msg2) {
    if (msg2) {
        fprintf(stderr,"ERROR - %s %s\n", msg1, msg2);
    }
    else {
        fprintf(stderr,"ERROR - %s\n", msg1);
    }
    return;
}

/*********************************************************
 * print an error message on stderr followed by system message
 *********************************************************/

void SysErrMsg(char * msg1, char * msg2) {
    if (msg2) {
        fprintf(stderr,"ERROR - %s %s; ", msg1, msg2);
    }
    else {
        fprintf(stderr,"ERROR - %s; ", msg1);
    }
    perror(NULL);
    return;
}
