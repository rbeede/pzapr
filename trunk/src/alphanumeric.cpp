#include "alphanumeric.h"  // Only include, all others should be in the .h file


std::string currPassword_ALPHANUMERIC;

std::string lastPassword_ALPHANUMERIC;


void initializePasswordGenerator_brute(const int rank, const int numProcesses) {
	
}


std::string getNextPassword_brute() {
	const string currValue = currPassword_ALPHANUMERIC;
	
	// Increment for next time around
	incrementPassword();
	
	return currValue;
}


void incrementPassword() {
	if(currPassword_ALPHANUMERIC == lastPassword_ALPHANUMERIC) {
		// Reached end of range so flag it so
		currPassword_ALPHANUMERIC = string("");
	}
	
	if(string("") == currPassword_ALPHANUMERIC) {
		return string("");
	}
	
	
}


void incrementCharacter(const int position) {
	
}
