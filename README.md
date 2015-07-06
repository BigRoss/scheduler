# scheduler

University Assignment to create a simple dispatcher for processes. It can handle real time processes, 
processes with different priorities, and also manages memory/IO. It uses the FIFO algorithm for the real time
processes and a round-robin technique with the 3 level feedback queues. 

Create processes through user input in this format (all integeres seperated by whitepace) :
arrival time, priority, cpu time required, mbytes required, printers required, scanners required, modems required, cds required.

