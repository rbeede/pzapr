/*
 * Rodney Beede
 *
*/

#ifndef _ALPHANUMERIC_H
#define _ALPHANUMERIC_H

#include <string>

void initializePasswordGenerator_brute(const int rank, const int numProcesses);
std::string getNextPassword_brute();


#endif /* _ALPHANUMERIC_H */