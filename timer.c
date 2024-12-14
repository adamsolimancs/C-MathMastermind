/*
 * timer.c
 *
 *  Created on: Dec 13, 2024
 *  Author: Adam Soliman
 */
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include<signal.h>
int secs = 0;

/**
 *	Main method for the program, a 5 second ascending timer. Kills parent process when time is up.
 */
int main(int argc, char **argv) {
	sleep(1);
	// signal(SIGINT, handle_sigint);

	printf("\n");
	while (secs < 5) {
		printf("Time:%i\n", secs);
		fflush(0);
		sleep(1);
		secs++;
		printf("\r\b\r");
	}
	pid_t parent_pid = getppid();
	kill(parent_pid, SIGINT);

	return 0;
}
