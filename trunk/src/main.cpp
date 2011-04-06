/*
 * Neelam Agrawal
 * Rodney Beede
 * Yogesh Shrikant Virkar
 *
 * CSCI 5576 - High Perf Sci Computing
 * 
 * Assignment:  Final Group Project
 * 
 * Due:  Wed, May 4, 2011
 * 
 * Notes:  log file argument should be directory path and prefix of the filename.  The GLOBAL_mpiRuntimeInfo->mpi_processor_name
 *			will be added to the end (with only system friendly filename chars) along with "_rankXofY" for you
 *			You could use a scheduler batch file or scheduler env variables (dependent on system) to include the job id.
 *			Ex:  mpirun ... pzapr.bin /scratch/${COBALT_JOBID}_pzapr_
 *				This would result in something like /scratch/54310_pzapr_Processor_0_0_0_1_in_a_4__4__2__2__mesh_rank2of4.log
 * 
*/

#include <iostream>
#include <sstream>
#include <unistd.h>

#include "logger.h"
#include "stringutils.h"
#include "mpi_support.h"  // defines constant global GLOBAL_mpiRuntimeInfo

#include "alphanumeric.h"
#include "dictionary.h"
#include "decryptzip.h"

using namespace std;

extern char **environ;  // May change if you modify the environment in your code

// Global variables
Logger * logger;


string makeValidFilename(const string origString) {
	string str(origString);
	
	size_t position;
	
	while(string::npos != (position = str.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_"))) {
		str[position] = '_';
	}
	
	return str;
}


/*
 * isBruteForce decides which init function will be called
 * rank is the zero-indexed rank
 * numProcesses is the number of processes (starting at 1)
 * dictionaryFilePathname Set to NULL if using isBruteForce, otherwise the path to the file
 * 
*/
void initializePasswordGenerator(const bool isBruteForce, const int rank, const int numProcesses, const char * const dictionaryFilePathname) {
	if(isBruteForce) {
		initializePasswordGenerator_brute(rank, numProcesses);
	} else {
		initializePasswordGenerator_dictionary(rank, numProcesses, dictionaryFilePathname);
	}
}


/**
 * Calls appropriate method to retrieve the next password for this process
 *
*/
std::string getNextPassword(const bool isBruteForce) {
	if(isBruteForce) {
		return getNextPassword_brute();
	} else {
		return getNextPassword_dictionary();
	}
}


/*
 * argc - Length of argv
 * argv - Array of length argc that holds passed in arguments
 * 			Element at position argv[0] is usually the path (relative or absolute) to the binary executable for this
 * 			process, but may not be set at all or just some made-up value.
 * envp - Obsolete and no longer POSIX compliant.  If anything were to modify the env this would have pointed to stale
 *			data so instead use the "extern char **environ" pointer.
*/
int main (const int argc, const char * const argv[]) {
	// Output to STDOUT so we at least know it is alive just in case our logging file fails
	cout << "MPI process on processor called '" << GLOBAL_mpiRuntimeInfo->mpi_processor_name << "' has rank "
		<< GLOBAL_mpiRuntimeInfo->mpi_rank << " of " << GLOBAL_mpiRuntimeInfo->mpi_num_proc << " processes" << endl;
	cout << flush;

		
	if(argc < 4) {
		cerr << "Insufficent args" << endl;
		cerr << "Usage:  " << argv[0] << " <log file path and filename prefix> <zip file pathname> BRUTE|DICTIONARY [<dictionary file pathname>]" << endl;
		MPI_Finalize();
		return 0;
	}
		

	const char * const logFileBasePathname = argv[1];
	const char * const zipFilePathname = argv[2];
	const bool isBruteForce = ('B' == argv[3][0]);
	const char * const dictionaryFilePathname = (!isBruteForce && argc > 4) ? argv[4] : NULL;

	if(!isBruteForce && NULL == dictionaryFilePathname) {
		cerr << "Missing dictionary file pathname argument!" << endl;
		MPI_Finalize();
		return 0;
	}
	
	// Create a valid log file pathname with extra information so each process has a separate log file
	string completeLogFilePathname = string(logFileBasePathname) + "_" + makeValidFilename(GLOBAL_mpiRuntimeInfo->mpi_processor_name)
		+ "_rank" + to_string(GLOBAL_mpiRuntimeInfo->mpi_rank) + "of" + to_string(GLOBAL_mpiRuntimeInfo->mpi_num_proc)
		+ ".log";
	logger = new Logger(completeLogFilePathname);
	
	logger->log(to_string(GLOBAL_mpiRuntimeInfo->mpi_processor_name) + " rank " + to_string(GLOBAL_mpiRuntimeInfo->mpi_rank) + " of " + to_string(GLOBAL_mpiRuntimeInfo->mpi_num_proc));
	
	// Helpful for debugging
	for(int i = 0; i < argc; i++) {
		logger->log("argv[" + to_string(i) + "] == " + to_string(argv[i]));
	}

	
	initializePasswordGenerator(isBruteForce, GLOBAL_mpiRuntimeInfo->mpi_rank, GLOBAL_mpiRuntimeInfo->mpi_num_proc, dictionaryFilePathname);
	
	initDecryptEngine(zipFilePathname);

	
	// Listen (non-blocking) for a signal from another process in case it found the solution
	int solutionFoundByRank = -1;
	MPI_Request mpi_request;
	MPI_Irecv(&solutionFoundByRank, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &mpi_request);
	
	// Loop until we have success, run out of passwords to try, or are told another process found it already
	bool attemptSuccessful = false;
	string password;
	long numberAttempts = 0;
	while(!attemptSuccessful) {
		password = getNextPassword(isBruteForce);
		
		// A little progress indicator but not too often
		if(numberAttempts % 1000 == 0) {
			logger->log("Rank " + to_string(GLOBAL_mpiRuntimeInfo->mpi_rank) + " has made " + to_string(numberAttempts)
							+ " attempts");
		}
		
		if(password == string("")) {
			logger->log("Ran out of passwords to try");
			break;
		}

		attemptSuccessful = attemptPassword(password);  // This calls our decrypt engine to make the attempt
		
		if(!attemptSuccessful) {
			numberAttempts++;
		
			// Non-blocking check to see if another process has sent a signal that if found the solution :)
			int receivedFlag = 0;
			MPI_Test(&mpi_request, &receivedFlag, MPI_STATUS_IGNORE);
			
			if(receivedFlag) {
				// solutionFoundByRank has been filled in
				logger->log(string("Another process with rank ") + to_string(solutionFoundByRank) + " has found the solution");
				break;
			}
		}
	}

	// Useful for performance data gathering
	logger->log(to_string(numberAttempts) + " attempts made by process " + to_string(GLOBAL_mpiRuntimeInfo->mpi_num_proc));


	// Record the final result
	if(attemptSuccessful) {
		logger->log("Found the solution:\t'" + password + "'");
	
		logger->log("Notifying all other processes that I found the solution");
		
		// Tell all other processes that I found the solution
		for(int i = 0; i < GLOBAL_mpiRuntimeInfo->mpi_num_proc; i++) {
			if(GLOBAL_mpiRuntimeInfo->mpi_rank == i) {
				continue;  // No point in talking to self
			}
			
			// Use non-blocking send since it is possible that some processes have already exited because they tried
			//	all the possible solutions and didn't find one.
			MPI_Isend((void *) &GLOBAL_mpiRuntimeInfo->mpi_rank, 1, MPI_INT, i, 0, MPI_COMM_WORLD, NULL);
		}
	} else {
		logger->log("This process (" + to_string(GLOBAL_mpiRuntimeInfo->mpi_rank) + ") didn't find the solution");		
	}
		
	
	logger->log("Process with rank " + to_string(GLOBAL_mpiRuntimeInfo->mpi_rank) + " completed");
	
	// Clean up memory and MPI and exit
	MPI_Request_free(&mpi_request);
	delete(logger);
	delete(GLOBAL_mpiRuntimeInfo);
	MPI_Finalize();
	return 0;
}
