/*
 * Yogesh Virkar
 * 
*/

#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <string>
#include <iostream>
#include <istream>
#include <fstream>
#include <ctype.h>

#include "mpi.h"
#include "logger.h"
#include "stringutils.h"

/*This function sets the offset for each process and the number of passwords to handle for each process*/
void initializePasswordGenerator_dictionary(const int rank, const int numProcesses, const char * const dictionaryFilePathname);

/*This function returns the next psswd to try for, and returns NULL when all passwords are returned*/
std::string getNextPassword_dictionary();

/*This function counts the total number of passwords present in the dictionary
 *and also removes the passwords which are not alphanumeric*/
int count_Number_Of_Words();

/*This function calculates the displacement of each word from the start of the dictionary file*/
void get_Displacement_of_Each_Word(int* dispWords);


#endif /* _DICTIONARY_H */