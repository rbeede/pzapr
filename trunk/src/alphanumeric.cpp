#include "alphanumeric.h"  // Only include, all others should be in the .h file


extern Logger * logger;

long startRange_ALPHANUMERIC;
long endRange_ALPHANUMERIC;
long currPos_ALPHANUMERIC;


void initializePasswordGenerator_brute(const int rank, const int numProcesses) {
	const long numPossiblePasswords = calculateNumberPossible(NUM_POSS_PW_ALPHABET, MAX_PASSWORD_LENGTH);
	logger->log("Number of possible passwords when there are " + to_string(NUM_POSS_PW_ALPHABET) + 
				" possible character values and passwords of up to " + to_string(MAX_PASSWORD_LENGTH) +
				" length is " + to_string(numPossiblePasswords));
	
	
	// We need to calculate a mostly even distribution of work for all processes
	// In the event that the number of processes doesn't evenly divide into the number of brute force passwords to
	//	attempt the last process will have a little bit more work
	const double blockSize = (double)numPossiblePasswords / (double)numProcesses;
	
	startRange_ALPHANUMERIC = ceil(blockSize * (double)rank);
	endRange_ALPHANUMERIC = floor((double)startRange_ALPHANUMERIC + blockSize);
	
	if(0 == startRange_ALPHANUMERIC) {
		// We adjust after calculating myEndRange
		startRange_ALPHANUMERIC = 1;  // 0 implies empty password which we don't allow
	}
	if((rank - 1) == numProcesses && numPossiblePasswords != endRange_ALPHANUMERIC) {
		// Need to pick up uneven division although it should be very close
		logger->log("Adjusting end range from " + to_string(endRange_ALPHANUMERIC) + " to true end range of " +
					to_string(numPossiblePasswords));
		endRange_ALPHANUMERIC = numPossiblePasswords;
	}

	currPos_ALPHANUMERIC = startRange_ALPHANUMERIC - 1;
	
	logger->log("Process " + to_string(rank) + " is doing brute password range of " + to_string(startRange_ALPHANUMERIC) +
				" to " + to_string(endRange_ALPHANUMERIC));
}


std::string getNextPassword_brute() {
	currPos_ALPHANUMERIC++;
	
	if(currPos_ALPHANUMERIC > endRange_ALPHANUMERIC) {
		return string("");
	} else {
		return passwordFromRangePosition(currPos_ALPHANUMERIC);
	}
}


long calculateNumberPossible(const int possibleCombinations, const int characters) {
	if(characters <= 0) {
		return 0;
	}
	
	return pow(possibleCombinations, characters) + calculateNumberPossible(possibleCombinations, characters - 1);
}


/**
 *	This algorithm allows us to determine a password for an arbitrary position in a range of passwords.
 *
 *	Let position represent the placement of a password in a range of 1 to "all possible passwords of length 1 to MAX_PASSWORD_LENGTH"
 *
 *	Let PASSWORD_ALPHABET represent all the possible characters that can be used in a password with the special requirement
 *		that PASSWORD_ALPHABET[0] = '\0'.
 *	The array indexes represent possible numerical factors that will be used in factoring position later.  The 0 factor
 *		is the '\0' character which is required since we can't use factors of 0 later.
 *
 *	
 *
 *
*/
std::string passwordFromRangePosition(const long position) {
	// Figure out how long the password must be at the given position
	int passwordLength = MAX_PASSWORD_LENGTH;  // assume the worst
	while(calculateNumberPossible(NUM_POSS_PW_ALPHABET, passwordLength) >= position) {
		passwordLength--;
	}
	passwordLength++;

	string password("");
	
	
	// Holds character index position in PASSWORD_ALPHABET for factors c1, c2, c3, etc...
	//	Last element holds first character index (aka factor), next last is second, etc until first element which is
	//	last character index in PASSWORD_ALPHABET
	int factors[passwordLength];
	for(int i = 0; i < passwordLength; i++) {
		factors[i] = 0;
	}

	for(int j = passwordLength - 1; j >= 0; j--) {
		long runningTotal = 0;
		for(int i = passwordLength - 1; i > j; i--) {
			runningTotal += factors[i] * pow(NUM_POSS_PW_ALPHABET, i);
		}
		long remainder = position - runningTotal;
	
		int factor = remainder / pow(NUM_POSS_PW_ALPHABET, j);

		// Did we hit an edge where we may be rolling over?
		if(0 == factor) {
			// Need to borrow from previous factor calculated, which may have to borrow as well

			// Need to force previous factor to be 1 less
			// previous factor is in factors[j+1]
			for(int prevFacIdx = j+1; prevFacIdx < passwordLength; prevFacIdx++) {
				factors[prevFacIdx]--;
				if(factors[prevFacIdx] == 0) {
					// Borrow one from next factor
					factors[prevFacIdx] = NUM_POSS_PW_ALPHABET;  // set borrowed amount
					continue;  // next loop around will adjust next factor we borrowed from
				} else {
					// Done borrowing
					break;
				}
			}

			
			// Retry the current factor again
			j++;
			continue;
		}
		
		
		factors[j] = factor;
	}
	
	for(int i = passwordLength - 1; i >= 0; i--) {
		password.append(1, PASSWORD_ALPHABET[factors[i]]);
	}
	

	return password;
}
