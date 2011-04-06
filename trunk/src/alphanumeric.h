/*
 * Rodney Beede
 *
*/

#ifndef _ALPHANUMERIC_H
#define _ALPHANUMERIC_H

#include <string>
#include <math.h>
#include <algorithm>

#include "logger.h"
#include "stringutils.h"


const int MAX_PASSWORD_LENGTH = 7;

// All the possible password characters we try for brute force, this mapping makes it easier to do ranges
// Note that null character isn't really used as a possible valid character in the password, just the string terminator
// Note that the order isn't important (except for \0), you could make certain characters tried first by placing them
//	at the beginning
const char PASSWORD_ALPHABET[] = {'\0',  // null character must be at index 0
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
	'x', 'y', 'z'
	};
const int PASSWORD_ALPHABET_length = 63;  // get this right
const int NUM_POSS_PW_ALPHABET = PASSWORD_ALPHABET_length - 1;  // Exclude null character


/**
 * Returns how many possible combinations based on alphabet length and number of characters
 *
 *
*/
long calculateNumberPossible(const int possibleCombinations, const int numCharacters);


/**
 * Initializes global variables for later calls to getNextPassword_brute.
 *
 * You must call this before getNextPassword_brute
 *
 * Relies on global variables declared in header file for possible characters and max password length
 *
 *
 * rank - The rank of the current process, determines start position
 *
 * numProcesses - Number of processes for calculating range of passwords for this process
 *
 *
*/
void initializePasswordGenerator_brute(const int rank, const int numProcesses);


/**
 * Returns a string with the next password in the range or string("") if all have been given out.
 *
 * You must call initializePasswordGenerator_brute before calling this method or undefined behavior occurs
 *
*/
std::string getNextPassword_brute();


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
 *	Let NUM_POSS_PW_ALPHABET = length(PASSWORD_ALPHABET) - 1   (-1 to exclude \0)
 *
 *	Then f_passwordLength(position) = calculateNumberPossible(NUM_POSS_PW_ALPHABET, passwordLength) < position
 *				Such that starting from MAX_PASSWORD_LENGTH and decrementing passwordLength would satisfy the above
 *
 *	Thus f_password(position, f_passwordLength(position)) =	
 *								factors of position in the form
 *		factor1 * (NUM_POSS_PW_ALPHABET)^(passwordLength - 1) + 
 *		factor2 * (NUM_POSS_PW_ALPHABET)^(passwordLength - 2) +
 *			...
 *		factor_n-1 * (NUM_POSS_PW_ALPHABET)^(passwordLength - (passwordLength-1)) +
 *		factor_n * (NUM_POSS_PW_ALPHABET)^(0)
 *
 *	with no factor# allowed to be 0.  When a factor does arrive at zero the algorithm must borrow a value from a previous
 *	factor be decrementing it.  This may continue recursively as needed.
 *
 *
 * The factor#'s correspond to the index position of the character in PASSWORD_ALPHABET
 *
 *
*/
std::string passwordFromRangePosition(const long position);


#endif /* _ALPHANUMERIC_H */