/*
 * Rodney Beede
 * CSCI 5576 - High Perf Sci Computing
 * 
 * Assignment:  
 * 
 * Due:  
 * 
 * Notes:  
 * 
*/

#include <iostream>

#include "mpi_support.h"  // defines constant global GLOBAL_mpiRuntimeInfo

using namespace std;


/*
 * argc - Length of argv
 * argv - Array of length argc that holds passed in arguments
 * 			Element at position argv[0] is usually the path (relative or absolute) to the binary executable for this
 * 			process, but may not be set at all or just some made-up value.
 * envp - Environment variables for this process.  Last element of array is indicated by value of NULL
*/
int main (const int argc, const char * argv[], const char * envp[]) {
	cout << "MPI process on processor called '" << GLOBAL_mpiRuntimeInfo->mpi_processor_name << "' has rank "
		<< GLOBAL_mpiRuntimeInfo->mpi_rank << " of " << GLOBAL_mpiRuntimeInfo->mpi_num_proc << " processes" << endl;
	cout << flush;

		
	if(argc < 1) {
		cerr << "Insufficent args" << endl;
		cerr << "Usage:  " << argv[0] << " --requiredArg <value> [--optional]" << endl;
		MPI_Finalize();
		return 0;
	}
		
	
	for(int i = 0; envp[i]; i++) {  // do while current array item isn't NULL
		cout << envp[i] << endl;
	}
		
	cout << "==========" << endl;
	
	for(int i = 0; i < argc; i++) {
		cout << argv[i] << endl;
	}
	
	
	// Clean up memory and MPI and exit
	delete(GLOBAL_mpiRuntimeInfo);
	MPI_Finalize();
	return 0;
}
