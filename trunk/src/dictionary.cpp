#include "dictionary.h"  // Only include, all others should be in the .h file


std::string currPassword_DICTIONARY;

extern Logger * log;


void initializePasswordGenerator_dictionary(const int rank, const int numProcesses, const char * const dictionaryFilePathname) {
	log->log("Reading dictionary file at " + to_string(dictionaryFilePathname));
}

std::string getNextPassword_dictionary() {
	return std::string("");
}