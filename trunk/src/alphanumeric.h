/*
 * Rodney Beede
 *
*/

#ifndef _ALPHANUMERIC_H
#define _ALPHANUMERIC_H

#include <string>

using namespace std;

void initializePasswordGenerator_isBruteForce(const int rank, const int numProcesses);
string getNextPassword_brute();


#endif /* _ALPHANUMERIC_H */