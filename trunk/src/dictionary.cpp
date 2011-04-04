/*
 *	Yogesh Virkar
 *
*/

#include "dictionary.h"  // Only include, all others should be in the .h file

extern Logger * logger;

using namespace std;


/*The dictionary_file handler for reading purposes*/
ifstream dictionaryFile;

/* The position in the file where each process starts reading */
streampos offset;

/* The number of words each process tries out */ 
int perProcess_WordCount;


int count_Number_Of_Words(std::ifstream &myfile)
{
	int numWords=0;
	string line;

	if(myfile.is_open())
	{
		while(myfile.good())
		{
			getline(myfile, line);
			++numWords;
		}
	}
	else
	{
		cout<<"Unable to open file!"<<endl;
	}
	
	return numWords;
}

void get_Displacement_of_Each_Word(std::ifstream &myfile, int* dispWords)
{
	string line;
	int i=1;
	dispWords[0] = 0;

	if(myfile.is_open())
	{
		while(!(myfile.eof()))
		{
			getline(myfile, line);
			dispWords[i] = dispWords[i-1] + line.size() + 2;
			i++;
		}
	}
	else
	{
		cout<<"Unable to open file!"<<endl;
	}
	
}

void initializePasswordGenerator_dictionary(const int rank, const int numProcesses, const char * const dictionaryFilePathname) {

	int tempval;
	
	logger->log(rank + ": Reading dictionary file at " + to_string(dictionaryFilePathname));

	/*Calculate total words in the dictionary file*/
	dictionaryFile.open(dictionaryFilePathname);
	int totalWords = count_Number_Of_Words(dictionaryFile);
	dictionaryFile.close();
		
	perProcess_WordCount = totalWords/numProcesses;
	tempval = totalWords%numProcesses;

	
	int* dispWords = new int[totalWords];
	
	/*Find the displacement of each word from the start of the file*/
	dictionaryFile.open(dictionaryFilePathname);
	get_Displacement_of_Each_Word(dictionaryFile, dispWords);
	dictionaryFile.close();

	/*If the number of passwords not divisible by the number of processes, 
	then evenly distribute the remaining ones*/
	if(rank<tempval)
		perProcess_WordCount++;


	int index = (rank<tempval)?(rank*perProcess_WordCount):((tempval)*(perProcess_WordCount+1) + (rank-tempval)*perProcess_WordCount);

	/*The starting position for each process is set here*/
	offset = (streampos) dispWords[index];
	
	dictionaryFile.open(dictionaryFilePathname);
	dictionaryFile.seekg(offset);

	string nextPassword;

	/* Displaying Password list for rank 5*/
	/*
	if(rank==5)
	{
		cout<< "Process " << rank << ": displaying its psswd list----" << endl;
		while((nextPassword=getNextPassword_dictionary()) != "")
		{
			cout << nextPassword << endl;
		}
	}
	*/

}

std::string getNextPassword_dictionary() {
	string line("");
	
	if(perProcess_WordCount>0)
		getline(dictionaryFile, line);
	perProcess_WordCount--;
	
	return line;
}