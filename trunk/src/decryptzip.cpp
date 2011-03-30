#include "decryptzip.h"  // Only include, all others should be in the .h file



extern Logger * log;

// Global variables that should only be used by decryptzip
char * zipfileBytes;

void initDecryptEngine(const char * const zipFilePathname) {
	//read in zip file and store content in global variable	
	std::fstream zipfileStream;
	zipfileStream.open(zipFilePathname);
	
	//get byte size of zip file
	if(zipfileStream.is_open())
	{
		long sizeofzip;
		zipfileStream.seekg(0,std::ios::end);
		sizeofzip = zipfileStream.tellg();
		zipfileStream.seekg(0,std::ios::beg);

		//zipfileBytes = new unsigned char[SIZE OF ZIP];
		zipfileBytes = new char[sizeofzip];
	
		//read in zip file into memory
		zipfileStream.read(zipfileBytes,sizeofzip);

		// close the file
		zipfileStream.close();
	}
	else {
		log->log("Unable to open the zip file");
		//TODO mpi abort call
	}
}

bool attemptPassword(const std::string password) {
	return false;
}
