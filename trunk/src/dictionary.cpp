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


void initializePasswordGenerator_dictionary(const int rank, const int numProcesses, const char * const dictionaryFilePathname) 
{
	int tempval;

	dictionaryFileHandle.open(dictionaryFilePathname, ios::in);
	
	logger->log(to_string(rank) + ": Reading dictionary file at " + to_string(dictionaryFilePathname));

	if(!dictionaryFileHandle.is_open()) 
	{
		logger->log("ERROR OPENING DICTIONARY FILE");
		MPI_Abort( MPI_COMM_WORLD, 255 );
		return;
	}
	

	/*------Counts the total number of words in the dictionary file------*/
	const int totalWords = count_Number_Of_Words(dictionaryFileHandle);
	dictionaryFileHandle.seekg(0, ios::bed);
	dictionaryFileHandle.clear();
	/*-------------------------------------------------------------------*/


	logger->log("There were " + to_string(totalWords) + " total dictionary words");
	

	perProcess_WordCount = totalWords / numProcesses;
	tempval = totalWords % numProcesses;

	int* dispWords = new int[totalWords];
	
	/*----Finds the displacement of each word from the start of the file----*/
	get_Displacement_of_Each_Word(fileHandle2, dispWords);
	dictionaryFileHandle.seekg(0, ios::bed);
	dictionaryFileHandle.clear();
	/*----------------------------------------------------------------------*/


	/* If the number of passwords not divisible by the number of processes, 
	 * then evenly distribute the remaining ones.
	 * Important to balance load evenly amongst all the processes*/
	if(rank<tempval)
		perProcess_WordCount++;
	/*-------------------------------------------------------------------*/


	/*------------------------------Set the index according to the rank of the processor--------------------------------------------*/
	/*-------------------This index decides the position in the file from where this processor has to start reading-----------------*/
	int index = (rank<tempval)?(rank*perProcess_WordCount):((tempval)*(perProcess_WordCount+1) + (rank-tempval)*perProcess_WordCount);
	/*----------------------------------------------------------------------------------------------------------------------------*/



	/*-----The starting position for each processor is set here-----*/
	offset = (streampos) dispWords[index];
	dictionaryFileHandle.seekg(offset);
	/*------------------------------------------------------------*/


	logger->log("Process " + to_string(rank) + ":  " + "total_words= " + to_string(totalWords) + "No. of Words I will crack: " + to_string(perProcess_WordCount));
}

std::string getNextPassword_dictionary() 
{
	string line("");
	
	if(perProcess_WordCount > 0) 
	{
		getline(dictionaryFileHandle, line);
		perProcess_WordCount--;
	}
	
	return line;
}
