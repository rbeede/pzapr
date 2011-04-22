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
 
#include "alphanumeric.h"

using namespace std;

extern char **environ;  // May change if you modify the environment in your code

// Global variables
Logger * logger;




/*
 * argc - Length of argv
 * argv - Array of length argc that holds passed in arguments
 * 			Element at position argv[0] is usually the path (relative or absolute) to the binary executable for this
 * 			process, but may not be set at all or just some made-up value.
 * envp - Obsolete and no longer POSIX compliant.  If anything were to modify the env this would have pointed to stale
 *			data so instead use the "extern char **environ" pointer.
*/
int main (const int argc, const char * const argv[]) {
	logger = new Logger("./test.log");

	initializePasswordGenerator_brute(0, 1);
	
	logger->log("Password at position 1,000,000 is " + passwordFromRangePosition(1000000));

	return 0;
}
