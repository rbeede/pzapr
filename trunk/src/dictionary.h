/*
 * Rodney Beede
 *
*/

#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <string>

#include "stringutils.h"

#include "logger.h"

void initializePasswordGenerator_dictionary(const int rank, const int numProcesses, const char * const dictionaryFilePathname);
std::string getNextPassword_dictionary();


#endif /* _DICTIONARY_H */