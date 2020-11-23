all: process fcfs

process: sigtrap.c
	gcc sigtrap.c -o process

fcfs: fcfs.c
	gcc fcfs.c pcb.c -o fcfs -fsanitize=address