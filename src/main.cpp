/*
 * main.cpp
 *
 *  Created on: Oct 4, 2016
 *      Author: justin
 *      UB ID: 0962344
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <string.h>
#include <algorithm>

#define MAX_LINE  80
using namespace std;

int main() {
	bool should_run = true;
	// Main Loop
	while (should_run) {
		printf("UBos>");
		fflush(stdout);
		string inputs[(MAX_LINE/2) + 1];
		int i = 0;
		// Get input
		while(cin.get() != '\n') {
			cin.unget();
			cin >> inputs[i++];
		}
		// Check to see if user wants to exit
		if(inputs[0] == "exit" || inputs[0] == "EXIT" || inputs[0] == "Exit") {
			should_run = false;
		} else if(inputs[0] == "selection") {
			cout << "Here" << endl;
		} else {
			// Assign parameters from string [] to char * []
			char * parameters[(MAX_LINE/2) + 1];
			for (int j = 0; j < (MAX_LINE/2) + 1; j++) {
				if(inputs[j] == ""){ // If no more arguments
					parameters[j] = NULL; // End char * [] with NULL
					break;
				}
//				inputs[j].erase(remove(inputs[j].begin(), inputs[j].end(), '\"'), inputs[j].begin());
				parameters[j] = &inputs[j][0u]; // Convert string to char *
			}
			// Forking execution
			pid_t childPID;
			childPID = fork();
			if (childPID == 0) { // Child Process
				int returnStatus;
				if(parameters[0] == NULL) { // If no arguments
					returnStatus = execvp(parameters[0], parameters);
				} else { // If arguments
					returnStatus = execvp(parameters[0] , parameters);
					if (returnStatus != 0) { // Error in Child process
						cout << "Error." << endl << "Child process returned: " << returnStatus << endl;
						exit(0);
					}
				}
			} else { // Parent Process
				pid_t otherPID;
				do { // Keep waiting until Child Process finishes
					otherPID = wait(&childPID);
					if(otherPID != childPID) { // Child Process finished
//						cout << "Child Process finished" << endl;
						break;
					}
				} while (otherPID != childPID);
			}
		}
	}
	return 0;
}


