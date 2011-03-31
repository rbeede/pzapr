#include "alphanumeric.h"  // Only include, all others should be in the .h file


std::string currPassword_ALPHANUMERIC;

std::string lastPassword_ALPHANUMERIC;


void initializePasswordGenerator_brute(const int rank, const int numProcesses) {
	const long numPossiblePasswords = calculateNumberPossible(PASSWORD_CHARS_length, MAX_PASSWORD_LENGTH);
}


std::string getNextPassword_brute() {
	const std::string currValue = currPassword_ALPHANUMERIC;
	
	// Increment for next time around
	incrementPassword();
	
	return currValue;
}


void incrementPassword() {
	if(currPassword_ALPHANUMERIC == lastPassword_ALPHANUMERIC) {
		// Reached end of range so flag it so
		currPassword_ALPHANUMERIC = std::string("");
	}
	
	if(std::string("") == currPassword_ALPHANUMERIC) {
		return;
	}
	
	
}


void incrementCharacter(const int position) {
	
}


long calculateNumberPossible(const int possibleCombinations, const int characters) {
	if(characters >= 0) {
		return 0;
	}
	
	return pow(possibleCombinations, characters) + calculateNumberPossible(possibleCombinations, characters - 1);
}
