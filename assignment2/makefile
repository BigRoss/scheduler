# MAKEFILE: Alexander Ling 430391570 - Operating Systems Project 2
# Use this to compile all of the relevant source files together to create the dispatcher executable named hostd
all: hostd.c
	gcc -g -Wall -std=gnu99 -pedantic hostd.c pcb.c mab.c rsrc.c -o hostd

clean:
	$(RM) hostd
