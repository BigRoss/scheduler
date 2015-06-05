# MAKEFILE: Alexander Ling 430391570
all: hostd.c
	gcc -Wall -std=gnu99 -pedantic hostd.c pcb.c mab.c rsrc.c -o hostd


clean:
	$(RM) hostd
