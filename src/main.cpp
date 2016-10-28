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
#include <sstream>
#include <vector>

#define MAX_LINE  80
using namespace std;

string getOutputFromCommand(string command) {
	string output;
	FILE * stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	command.append(" 2>&1");
	int counter = 1;
	stream = popen(command.c_str(), "r"); // Execute the command, send output to a stream
	if (stream) {
	while (!feof(stream)) { // Keep going until eof is encountered
		if (fgets(buffer, max_buffer, stream) != NULL) {
			// Temporary string stream, converting int to string
			stringstream stream;
			stream << counter++ << " "; // 1.[space] etc..
			output.append(stream.str()); // Append stream as string
			output.append(buffer); // Append executable file name + \n
			// Reset stream and counter
			stream.flush();
		}
	}
	pclose(stream); // Close stream
	}
	return output;
}
vector<string> fillSelectionsFromFound(string filesFound, char delimiter) {
	vector<string> temp;
	stringstream stream(filesFound);
	string token;

	while(getline(stream, token, delimiter)) {
		temp.push_back(token.erase(0, 2));
	}
	return temp;
}
int main() {
	bool should_run = true;
	// Main Loop
	while (should_run) {
		printf("UBos>");
		fflush(stdout);
		string inputs[(MAX_LINE/2) + 1];
		char * parameters[(MAX_LINE/2) + 1];
		int i = 0;
		// Get input
		while(cin.get() != '\n') {
			cin.unget();
			cin >> inputs[i++];
		}
		// Check to see if user wants to exit
		if(inputs[0] == "exit" || inputs[0] == "EXIT" || inputs[0] == "Exit") {
			should_run = false;
		} else if(inputs[0] == "selection" && parameters[0] != NULL) {
			// Command will find all files executable by user
			string findFilesCommand = "ls -l | grep  '\\-..x' | awk '{print $NF}'";
			string filesFound = getOutputFromCommand(findFilesCommand);
			cout << "Enter Selection: " << endl << filesFound << endl;
			vector<string> executableFiles = fillSelectionsFromFound(filesFound, '\n');
//			cout << executableFiles[0] << endl << executableFiles[1] << endl << executableFiles[2] << endl;

			int userChoice;
			cin >> userChoice;
			if(userChoice > executableFiles.size() || userChoice < 1) {
				cout << "Invalid number" << endl;
				continue;
			}
			string fileExecuteCmd;
			ostringstream stringStream;
			stringStream << "./" << executableFiles[userChoice - 1];
			fileExecuteCmd = stringStream.str();

			parameters[0] = &fileExecuteCmd[0u];
			parameters[1] = NULL;
			break;
		} else {
			// Assign parameters from string [] to char * []
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



