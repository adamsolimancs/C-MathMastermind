/*
 * problem2.c
 *
 *  Created on: Dec 11, 2024
 *  Author: Adam Soliman
 */
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include<time.h>
#include<signal.h>
#include <stdbool.h>
#include <string.h>
/**
 * Handles signals, and if a signal is received it means that the child says time is up.
 * Parameters
 * 		sig - input signal
 */
void handle_sigint(int sig);

/*
 *	Returns a pointer to a new String array with double the size of the input array.
 *	Parameters:
 *		arr - input array
 *		currentSize - size of input array
 *	Return:
 *		char**  a pointer to a new String array with double the size of the input array.
 */
char** resizeStringArray(char **arr, int currentSize);

int rangeCap = 40;		// Largest integer in range (exclusive)
int qCount;				// number of questions (to be prompted for)
int incorrSize;			// manually tracking the size of the incorrect array
int i;					// loop variable
char **questions;		// array to store the questions
char **missedq;			// array to store the missed questions
int missed = 0;			// number of missed questions
bool cont;				// loop condition

/**
 * The main method is the program. It asks the user for a number of questions, and asks simple math expressions with numbers
 * in the range [0-50).
 */
int main(int argc, char **argv) {
	signal(SIGINT, handle_sigint);
	srand(time(NULL));	// resets RNG
	int num;			// user input answer
	pid_t pid;
	printf("How many questions would you like?\n");
	if (scanf("%d", &qCount) != 1 || qCount <= 0) {
		fprintf(stderr, "Invalid input\n");
		exit(EXIT_FAILURE);
	}
	questions = malloc(qCount * sizeof(char*));
	missedq = malloc(qCount * sizeof(char*));
	if (questions == NULL || missedq == NULL) {
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	int answers[qCount];		// array to store the answers
	char **incorrect = malloc(qCount * sizeof(char*));// array to store the incorrect answers
	if (incorrect == NULL) {
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	incorrSize = qCount;
	int numBricks = 0;			// number of bricked answers
	int correct[qCount];		// array to store the index of correct answers
	int numCorrect = 0;

	// Loop for questions
	for (i = 0; i < qCount; i++) {
		// Generate two random numbers in range: [0-rangeCap) for the equation
		int rnum1 = rand() % rangeCap;
		int rnum2 = rand() % rangeCap;
		int operator = rand() % 3;
		char question[20];
		switch (operator) {
		case 0:
			sprintf(question, "%d + %d", rnum1, rnum2);
			questions[i] = strdup(question);
			answers[i] = rnum1 + rnum2;
			break;
		case 1:
			sprintf(question, "%d * %d", rnum1, rnum2);
			questions[i] = strdup(question);
			answers[i] = rnum1 * rnum2;
			break;
		case 2:
			sprintf(question, "%d - %d", rnum1, rnum2);
			questions[i] = strdup(question);
			answers[i] = rnum1 - rnum2;
			break;
		default:
			fprintf(stderr, "Error generating question\n");
		}
		printf("What is %s?\n", question);
		pid = fork();
		if (pid == 0) {
			char *args[] = {"timer.exe", NULL};
			execve("C:/Users/adoma/eclipse-workspace/PProject5/timer.exe", args, NULL);
			perror("execve failed");
			exit(EXIT_FAILURE);
		}
		cont = true;
		while (num != answers[i] && cont) {
			if (scanf("%d", &num) != 1) {
				printf("Invalid input.\n");
				exit(EXIT_FAILURE);
			}
			if (num == answers[i]) {
				if (i == qCount - 1) {
					printf("Correct! Thanks for playing.\n");
				} else {
					printf("Correct! Next question...\n");
				}
				correct[numCorrect++] = i;
				kill(pid, SIGKILL);			// end timer process
			} else {
				if (cont == false) {
					break;
				}
				printf("%i is incorrect - try again\n", num);
				// Save the incorrect answer
				char buff[20];
				sprintf(buff, "%s != %d", questions[i], num);
				if (numBricks >= incorrSize) {
					char **newArr = resizeStringArray(incorrect, incorrSize);
					incorrect = newArr;
					incorrSize *= 2;
				}
				incorrect[numBricks++] = strdup(buff);
			}
		}

	}

	printf("\n-------------------\nYour results:\n");
	printf("Correct answers:\n");
	if (numCorrect == 0) {
		printf("You got none correct.");
	} else {
		for (int k = 0; k < numCorrect; k++) {
			int index = correct[k];
			char buff[20];
			sprintf(buff, "\t%s = %i\n", questions[index], answers[index]);
			printf(buff);
		}
	}
	if (numBricks == 0) {
		printf("You had no incorrect guesses!\n");
	} else {
		printf("Incorrect answers:\n");
		for (int k = 0; k < numBricks; k++) {
			printf("\t%s\n", incorrect[k]);
			free(incorrect[k]);  // free dynamic memory
		}
	}
	if (missed == 0) {
		printf("You got none wrong!\n");
	} else {
		printf("Missed questions:\n");
		for (int k = 0; k < missed; k++) {
			printf("\t%s\n", missedq[k]);
			free(missedq[k]);
		}
	}

	// Free allocated memory
	for (int i = 0; i < qCount; i++) {
		free(questions[i]);
	}
	free(questions);
	free(incorrect);
	free(missedq);
	return 0;

}

/**
 * Handles signals, and if a signal is received it means that the child says time is up.
 * Parameters
 * 		sig - input signal
 */
void handle_sigint(int sig) {
	if (i == qCount - 1) {
		printf("\nTime is up!");
	} else {
		printf("\nTime is up! Next question...\n");
	}
	missedq[missed] = questions[i];
	missed++;
	cont = false;		// to break the loop
}

/*
 *	Returns a pointer to a new String array with double the size of the input array.
 *	Parameters:
 *		arr - input array
 *		currentSize - size of input array
 *	Return:
 *		char**  a pointer to a new String array with double the size of the input array.
 */
char** resizeStringArray(char **arr, int currentSize) {
	int newSize = currentSize * 2;
	char **newArr = (char**) malloc(newSize * sizeof(char*));
	if (newArr == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}
	for (int i = 0; i < currentSize; i++) {
		newArr[i] = arr[i];
	}
	free(arr);
	return newArr;
}
