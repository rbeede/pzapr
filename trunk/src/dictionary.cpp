/*
 *	Yogesh Virkar
 *
*/

#include "dictionary.h"  // Only include, all others should be in the .h file

std::string currPassword_DICTIONARY;

extern Logger * logger;

using namespace std;

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

	if(rank==1)
	{
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
		getline(dictionaryFile, line);
	perProcess_WordCount--;
	
	return line;
}

int main(int argc, char** argv)
{
	int         my_rank;       /* rank of process      */
    int         p;             /* number of processes  */
    int         source;        /* rank of sender       */
    int         dest;          /* rank of receiver     */

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	initializePasswordGenerator_dictionary(my_rank, p, "abc.txt");

	MPI_Finalize();

	return 0;
}