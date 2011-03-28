/*
 * Rodney Beede
 *
*/

#ifndef _ALPHANUMERIC_H
#define _ALPHANUMERIC_H

#include <string>


const int MAX_PASSWORD_LENGTH = 7;


void incrementCharacter(const int position);
void incrementPassword();
void initializePasswordGenerator_brute(const int rank, const int numProcesses);
std::string getNextPassword_brute();


#endif /* _ALPHANUMERIC_H */