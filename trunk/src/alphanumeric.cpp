#include "alphanumeric.h"  // Only include, all others should be in the .h file


extern Logger * logger;

std::string currPassword_ALPHANUMERIC;

std::string lastPassword_ALPHANUMERIC;


void initializePasswordGenerator_brute(const int rank, const int numProcesses) {
	const long numPossiblePasswords = calculateNumberPossible(POSSIBLE_PASSWORD_CHARS, MAX_PASSWORD_LENGTH);
	logger->log("Number of possible passwords when there are " + to_string(POSSIBLE_PASSWORD_CHARS) + 
				" possible character values and passwords of up to " + to_string(MAX_PASSWORD_LENGTH) +
				" length is " + to_string(numPossiblePasswords));
	
	
	// We need to calculate a mostly even distribution of work for all processes
	// In the event that the number of processes doesn't evenly divide into the number of brute force passwords to
	//	attempt the last process will have a little bit more work
	const double blockSize = (double)numPossiblePasswords / (double)numProcesses;
	
	long myStartRange = ceil(blockSize * (double)rank);
	long myEndRange = floor((double)myStartRange + blockSize);
	
	if(0 == myStartRange) {
		// We adjust after calculating myEndRange
		myStartRange = 1;  // 0 implies empty password which we don't allow
	}
	if((rank - 1) == numProcesses && numPossiblePasswords != myEndRange) {
		// Need to pick up uneven division although it should be very close
		logger->log("Adjusting myEndRange from " + to_string(myEndRange) + " to true end range of " +
					to_string(numPossiblePasswords));
		myEndRange = numPossiblePasswords;
	}
	
	logger->log("Process " + to_string(rank) + " is doing brute password range of " + to_string(myStartRange) +
				" to " + to_string(myEndRange));
				
	
	

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
	if(characters <= 0) {
		return 0;
	}
	
	return pow(possibleCombinations, characters) + calculateNumberPossible(possibleCombinations, characters - 1);
}



std::string passwordFromRangePosition(const long position) {
	const int passwordLength = (double)log(position) / (double)log(PASSWORD_CHARS_length);
	
	string password("");
	
	long calculation = position;
	
	for(int j = passwordLength - 1; j >= 0; j--) {
		for(int k = POSSIBLE_PASSWORD_CHARS; k != 0; k--) {
			if(k * pow(POSSIBLE_PASSWORD_CHARS, j) <= calculation) {
				password.append(1, PASSWORD_CHARS[k]);
				calculation = calculation - (k * pow(POSSIBLE_PASSWORD_CHARS, j));
				break;
			}
		}
	}
	
	return password;
}
