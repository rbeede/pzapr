#include "decryptzip.h"  // Only include, all others should be in the .h file


// Global variables that should only be used by decryptzip
unsigned char * zipfileBytes;


void initDecryptEngine(const char * const zipFilePathname) {
	//TODO read in zip file and store content in global variable
	
	std::ifstream zipfileStream;
	
	//TODO get byte size of zip file
	
	//TODO zipfileBytes = new unsigned char[SIZE OF ZIP];
	
	//TODO read in zip file into memory
	
	//TODO close the file
	
}

bool attemptPassword(const std::string password) {
	return false;
}