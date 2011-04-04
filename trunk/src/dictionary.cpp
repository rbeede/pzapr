/*
 *	Yogesh Virkar
 *
*/

#include "dictionary.h"  // Only include, all others should be in the .h file

//extern Logger * logger;

using namespace std;

/*The dictionary_file handler for reading purposes*/
ifstream alnumfile_read;

/* The position in the file where each process starts reading */
streampos offset;

/* The number of words each process tries out */ 
int perProcess_WordCount;

bool isAlphaNumeric(string s)
{
	int len = s.length();
	const char *str = new char[len];
	str = s.c_str();

	for(int i=0; i<len; i++)
	{
		if(!isalnum(str[i]))
			return false;
	}
	return true;
}

int count_Number_Of_Words(std::ifstream &myfile, std::ofstream &write_file)
{
	int numWords=0;
	string line;
	
	if(myfile.is_open())
	{
		while(myfile.good())
		{
			getline(myfile, line);
			
			/*If the password is alphanumeric, put in the alphanumeric file*/
			if(isAlphaNumeric(line))
			{
				++numWords;
				write_file << line << endl;
			}
		}
	}
	else
	{
		//logger->log("Unable to open file!");
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
		//logger->log("Unable to open file!");
	}
	
}

void initializePasswordGenerator_dictionary(const int rank, const int numProcesses, const char * const dictionaryFilePathname) {

	int tempval;

	/*File handler to read in original password dictionary file*/
	ifstream dictionaryFile;
	
	/*File handler to write a password file having only alphanumeric passwords*/
	ofstream alnumfile_write;
	
	alnumfile_write.open("alnumpsswd.txt");
	//logger->log(rank + ": Reading dictionary file at " + to_string(dictionaryFilePathname));

	/*Calculate total words in the dictionary file
	 *and remove the passwords which are not alphanumeric*/
	dictionaryFile.open(dictionaryFilePathname);
	
	int totalWords = count_Number_Of_Words(dictionaryFile, alnumfile_write);
	dictionaryFile.close();
	alnumfile_write.close();	

	perProcess_WordCount = totalWords/numProcesses;
	tempval = totalWords%numProcesses;

	
	int* dispWords = new int[totalWords];
	
	/*Find the displacement of each word from the start of the file*/
	alnumfile_read.open("alnumpsswd.txt");
	get_Displacement_of_Each_Word(alnumfile_read, dispWords);
	alnumfile_read.close();

	/*If the number of passwords not divisible by the number of processes, 
	then evenly distribute the remaining ones*/
	if(rank<tempval)
		perProcess_WordCount++;


	int index = (rank<tempval)?(rank*perProcess_WordCount):((tempval)*(perProcess_WordCount+1) + (rank-tempval)*perProcess_WordCount);

	/*The starting position for each process is set here*/
	offset = (streampos) dispWords[index];
	
	alnumfile_read.open("alnumpsswd.txt");
	alnumfile_read.seekg(offset);
	
	string nextPassword;

	/* Displaying Password list for rank 3*/
	
	if(rank==3)
	{
		cout << endl;
		cout<< "Process " << rank << ": displaying its psswd list----" << endl;
		while((nextPassword=getNextPassword_dictionary()) != "")
		{
			cout << nextPassword << endl;
		}
	}
	

}

std::string getNextPassword_dictionary() {
	string line("");
	
	if(perProcess_WordCount>0)
		getline(alnumfile_read, line);
	perProcess_WordCount--;
	
	return line;
}