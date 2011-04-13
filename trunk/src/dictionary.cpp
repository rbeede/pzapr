/*
 *	Yogesh Virkar
 *
*/

#include "dictionary.h"  // Only include, all others should be in the .h file

extern Logger * logger;

using namespace std;


/* The position in the file where each process starts reading */
streampos offset;

/* The number of words each process tries out */ 
int perProcess_WordCount;

/*File handler to read in original password dictionary file*/
ifstream dictionaryFileHandle;



int count_Number_Of_Words(std::ifstream &myfile)
{
	int numWords=0;
	string line;
	
	while(myfile.good())
	{
		getline(myfile, line);
		logger->log("DEBUG:\tdictionary line = '" + line + "'");
			
		++numWords;
	}

	return numWords;
}


void get_Displacement_of_Each_Word(std::ifstream &myfile, int * dispWords)
{
	string line;
	int i = 1;
	dispWords[0] = 0;

	while(!(myfile.eof()))
	{
		getline(myfile, line);
		dispWords[i] = dispWords[i-1] + line.size() + 1;
		i++;
	}
}


void initializePasswordGenerator_dictionary(const int rank, const int numProcesses, const char * const dictionaryFilePathname) {
	int tempval;
	ifstream fileHandle1, fileHandle2;

	dictionaryFileHandle.open(dictionaryFilePathname, ios::in);
	fileHandle1.open(dictionaryFilePathname, ios::in);
	fileHandle2.open(dictionaryFilePathname, ios::in);
	
	logger->log(to_string(rank) + ": Reading dictionary file at " + to_string(dictionaryFilePathname));

	// Calculate total words in the dictionary file
	if(!dictionaryFileHandle.is_open()) {
		logger->log("ERROR OPENING DICTIONARY FILE");
		MPI_Abort( MPI_COMM_WORLD, 255 );
		return;
	}
	
	const int totalWords = count_Number_Of_Words(fileHandle1);
	fileHandle1.close();


	perProcess_WordCount = totalWords / numProcesses;
	tempval = totalWords % numProcesses;

	
	int* dispWords = new int[totalWords];
	
	/*Find the displacement of each word from the start of the file*/
	//dictionaryFileHandle.open(dictionaryFilePathname, ios::in);
	get_Displacement_of_Each_Word(fileHandle2, dispWords);
	fileHandle2.close();

	/*If the number of passwords not divisible by the number of processes, 
	then evenly distribute the remaining ones*/
	if(rank<tempval)
		perProcess_WordCount++;


	int index = (rank<tempval)?(rank*perProcess_WordCount):((tempval)*(perProcess_WordCount+1) + (rank-tempval)*perProcess_WordCount);

	/*The starting position for each process is set here*/
	offset = (streampos) dispWords[index];
	//dictionaryFileHandle.open(dictionaryFilePathname, ios::in);
	dictionaryFileHandle.seekg(offset);
	
	string nextPassword;

	logger->log("\nProcess " + to_string(rank) + ": \n" + "total_words= " + to_string(totalWords) + "\nNo. of Words I will crack: " + to_string(perProcess_WordCount));

	/* DEBUG to remove */
	logger->log("Process " + to_string(rank) + ": displaying its psswd list----");

	while((nextPassword=getNextPassword_dictionary()) != string(""))
	{
		logger->log(nextPassword);
	}
}

std::string getNextPassword_dictionary() {
	string line("");
	
	if(perProcess_WordCount > 0) {
		getline(dictionaryFileHandle, line);
		perProcess_WordCount--;
	}
	
	return line;
}