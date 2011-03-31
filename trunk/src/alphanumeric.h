/*
 * Rodney Beede
 *
*/

#ifndef _ALPHANUMERIC_H
#define _ALPHANUMERIC_H

#include <string>
#include <math.h>

#include "logger.h"
#include "stringutils.h"


const int MAX_PASSWORD_LENGTH = 7;

// All the possible password characters we try for brute force, this mapping makes it easier to do ranges
// Note that null character isn't really used as a possible valid character in the password, just the string terminator
const char PASSWORD_CHARS[] = {'\0',  // null character must be at index 0
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
	'x', 'y', 'z'
	};
const int PASSWORD_CHARS_length = 63;  // get this right
const int POSSIBLE_PASSWORD_CHARS = PASSWORD_CHARS_length - 1;  // Exclude null character


void incrementCharacter(const int position);
void incrementPassword();

long calculateNumberPossible(const int possibleCombinations, const int characters);

void initializePasswordGenerator_brute(const int rank, const int numProcesses);
std::string getNextPassword_brute();

std::string passwordFromRangePosition(const long position);


#endif /* _ALPHANUMERIC_H */