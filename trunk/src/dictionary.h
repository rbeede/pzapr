/*
 * Rodney Beede
 * Yogesh Virkar
*/

#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <string>
#include <iostream>
#include <istream>
#include <fstream>
#include "mpi.h"
#include "logger.h"

void initializePasswordGenerator_dictionary(const int rank, const int numProcesses, const char * const dictionaryFilePathname);
std::string getNextPassword_dictionary();

/*The dictionary_file handler for reading purposes*/
ifstream dictionaryFile;

/* The position in the file where each process starts reading */
streampos offset;

/* The number of words each process tries out */ 
int perProcess_WordCount;

#endif /* _DICTIONARY_H */