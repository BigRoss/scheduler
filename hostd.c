/*******************************************************************
COMP3520 Assignment 2 Dispatcher - Alexander Ling 430391570

********************************************************************

version: 1.0 (exercise 7)
date:    December 2003
Code base from :  Dr Ian G Graham, ian.graham@griffith.edu.au

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
    FILE * inputliststream;
    PcbPtr inputqueue = NULL;       // input queue buffer
    PcbPtr currentprocess = NULL;   // current process
    PcbPtr currentRTprocess = NULL; // Current realtime process
    PcbPtr process = NULL;          // working pcb pointer
    PcbPtr rrQueue = NULL;          // Round robin queue
    PcbPtr fbQueue1 = NULL;         // Highest priority feedback queue
    PcbPtr fbQueue2 = NULL;         // Middle priority 
    PcbPtr fbQueue3 = NULL;         // Lowest priority fbQueue
    PcbPtr userQueue = NULL;        // User Queue
    PcbPtr rtQueue = NULL;          // Real-time queue
    int timer = 0;                  // dispatcher timer 
    //Make the MabPtr for the full memory, it will have only 960Mb of memory as the RT processes will take 64Mb from total memory.
    Mab memoryBlock;
    MabPtr memory = &memoryBlock;
    memory->offset = 64; //The starting offset is 64 as we leave the first 64 bits of memory for the RT processes
    memory->size = 960; 
    memory->allocated = 0;
    memory->next = NULL;
    memory->prev = NULL;
    //Here we initialize our real-time memory block which is permanently allocated for the use of real time processes.
    Mab rtMemoryBlock;
    MabPtr rtMemory = &rtMemoryBlock;
    rtMemory->offset = 0; //The starting offset is 0 for the real-time memory and it is always allocated 64Mb
    rtMemory->size = 64;
    rtMemory->allocated = 64;
    rtMemory->next = NULL;
    rtMemory->prev = NULL;

    //Create a rsrc ptr to show the available resources
    Rsrc resources;
    RsrcPtr availRes = &resources;
    availRes->printers = 2;
    availRes->scanners = 1;
    availRes->modems = 1;
    availRes->cds = 2;
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
    while(inputqueue != NULL || rrQueue != NULL || currentprocess != NULL || currentRTprocess != NULL || fbQueue1 != NULL || fbQueue2 != NULL || fbQueue3 != NULL || userQueue != NULL || rtQueue!= NULL){

        //i.Unload pending processes from the input queue: While (head-of-input-queue.arrival-time <= dispatcher timer)
        while(inputqueue!= NULL && inputqueue->arrivaltime <= timer){
            // Unload pending processes from the input queue and enqueue onto either realtime queue or user job queue:

            if(inputqueue->priority == 0){
                //If a real-time process is demanding resources don't enqueue it onto the RT queue, instead just dequeue it from the input queue and delete it
                if(inputqueue->req.printers != 0 || inputqueue->req.scanners != 0 || inputqueue->req.modems != 0 || inputqueue->req.cds != 0){
                    free(deqPcb(&inputqueue));
                    printf("real time jobs can't use resources - job deleted\n");
                }
                //Also if the RT process demands more than 64Mb of memory we delete it as RT processes may only be assigned 64Mb
                else if(inputqueue->mbytes > 64){
                    free(deqPcb(&inputqueue));
                    printf("excess memory request - job deleted\n");
                }
                else{
                    //Put on realtime queue after assigning it the memory block specially for real-time processes
                    inputqueue->memoryblock = rtMemory;
                    rtQueue = enqPcb(rtQueue, deqPcb(&inputqueue));
                }
            }
            else{
                //Check if the process asks for more resources than we have and delete the process if it does
                if(inputqueue->req.printers > 2 || inputqueue->req.scanners > 1 || inputqueue->req.modems > 1 || inputqueue->req.cds > 2){
                    free(deqPcb(&inputqueue));
                    printf("excess resource request - job deleted\n");
                }
                //Also check if the process wants more memory than we have delete it
                else if(inputqueue->mbytes > 960){
                    free(deqPcb(&inputqueue));
                    printf("excess memory request - job deleted\n");
                }
                else{
                    //This is a non-realtime process, hence we put it on the user job queue if it passes the memory/resource checks
                    userQueue = enqPcb(userQueue, deqPcb(&inputqueue));
                }
                
            } 
        }

        //  Unload pending processes from the user job queue if memory can be allocated resources are available
        while(userQueue!= NULL && memChk(memory, userQueue->mbytes) != NULL && rsrcChk(availRes, userQueue->req)){
            //dequeue the new process from user job queue 
            PcbPtr newProcess = deqPcb(&userQueue);

            //allocate memory and resources to the process
            MabPtr freeMem = memChk(memory, newProcess->mbytes); //Check if there is space for allocation
            newProcess->memoryblock = memAlloc(freeMem, newProcess->mbytes); //Allocate memory
            rsrcAlloc(availRes, newProcess->req);

            //Enqueue the process on the appropriate feedback queue
            if(newProcess->priority == 1){
                fbQueue1 = enqPcb(fbQueue1, newProcess);    
            }
            else if(newProcess->priority == 2){
                fbQueue2 = enqPcb(fbQueue2, newProcess);  
            }
            else{
                fbQueue3 = enqPcb(fbQueue3, newProcess);  
            }
        }

        // ii.If there is a real-time process, run it as it holds priority over the other normal processes
        if(currentRTprocess != NULL){
            // a. Decrement process remainingcputime;
            currentRTprocess->remainingcputime = currentRTprocess->remainingcputime - QUANTUM;
            // b. If times up:
            if(currentRTprocess->remainingcputime <= 0){
                //A. Send SIGINT to the process to terminate it;
                terminatePcb(currentRTprocess);
                //B. Free up process structure memory
                free(currentRTprocess);
                currentRTprocess = NULL;
            }
        }
        // iii.If a normal process is currently running:
        else if(currentprocess != NULL){
            // a. Decrement process remainingcputime;
            currentprocess->remainingcputime = currentprocess->remainingcputime - QUANTUM;

            // b. If times up:
            if(currentprocess->remainingcputime <= 0){
                //A. Send SIGINT to the process to terminate it;
                terminatePcb(currentprocess);
                //B. Free memory and resources allocated to the process;
                memFree(currentprocess->memoryblock);
                rsrcFree(availRes, currentprocess->req);
                //C. Free up process structure memory
                free(currentprocess);
                currentprocess = NULL;
            }
            //c.    else if it is a user process and other processes (that are "ready" or "suspended") are waiting in any of the queues
            // (other than the input queue or user job queue): 
            else if(fbQueue1 != NULL || fbQueue2 != NULL || fbQueue3 != NULL){
                //A. Send SIGTSTP to suspend it;
                currentprocess = suspendPcb(currentprocess);

                //B. Reduce the priority of the process (if possible) and enqueue it on the appropriate feedback queue
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

        // iv. If no process currently running && real time queue and feedback queue are not all empty: 
        if((currentprocess == NULL || currentRTprocess == NULL) && (fbQueue1 != NULL || fbQueue2 != NULL || fbQueue3 != NULL || rtQueue != NULL)){

            //Start a new real time process if there is one on the queue and there is not one currently running
            if(rtQueue != NULL){
                if(currentRTprocess == NULL){
                    currentRTprocess = startPcb(deqPcb(&rtQueue));    
                }
            }
            //Else start the normal processes, starting from the highest priority
            else if(fbQueue1 != NULL){
                currentprocess = startPcb(deqPcb(&fbQueue1));
            }
            else if(fbQueue2 != NULL){
                currentprocess = startPcb(deqPcb(&fbQueue2));
            }
            else if(fbQueue3 != NULL){
                currentprocess = startPcb(deqPcb(&fbQueue3));
            }

        }
        // v. sleep for one second;
        sleep(1);

        // vi. Increment dispatcher timer;

        timer = timer + QUANTUM;
            
        // vii. Go back to 4.                                        
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
