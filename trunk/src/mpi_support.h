/*
 * Common support functions and variables for using MPI in a C/C++ program
 *
 * Don't forget to call MPI_Finalize();
 *
 * Rodney Beede
 *
*/

#ifndef _MPI_SUPPORT_H
#define _MPI_SUPPORT_H



#include <string>

#include "mpi.h"


class MPI_Runtime_Info {
	public:
		const int mpi_rank;
		const int mpi_num_proc;
		const std::string mpi_processor_name;

		MPI_Runtime_Info(const int, const int, const char * const, const int);
};

MPI_Runtime_Info::MPI_Runtime_Info(const int param_mpi_rank, const int param_mpi_num_proc, const char * const param_mpi_processor_name, const int param_mpi_processor_name_len)
	: mpi_rank(param_mpi_rank), mpi_num_proc(param_mpi_num_proc), mpi_processor_name(std::string(param_mpi_processor_name, param_mpi_processor_name_len))
{
	// All class data members are const so all done
}

MPI_Runtime_Info * getMpiRuntimeInfo(const MPI_Comm mpiComm) {
	int mpi_rank;
	int mpi_num_proc;
	char mpi_processor_name[MPI_MAX_PROCESSOR_NAME];
	int mpi_processor_name_len;
	
	MPI_Comm_rank(mpiComm, &mpi_rank);
	MPI_Comm_size(mpiComm, &mpi_num_proc);	
	MPI_Get_processor_name(mpi_processor_name, &mpi_processor_name_len);
	// Ensure null terminated string since API doesn't say so
	mpi_processor_name[(mpi_processor_name_len < MPI_MAX_PROCESSOR_NAME) ? mpi_processor_name_len+1 : MPI_MAX_PROCESSOR_NAME-1] = '\0';
	
	return new MPI_Runtime_Info(mpi_rank, mpi_num_proc, mpi_processor_name, mpi_processor_name_len);
}



// Called for you by a constant global variable below
MPI_Runtime_Info * setupMPI(const MPI_Comm mpiComm) {
	//In accordance with MPI-2, it is valid to pass NULL in place of argc and argv.
	//http://publib.boulder.ibm.com/infocenter/clresctr/vxrx/index.jsp?topic=/com.ibm.cluster.pe432.mpisub.doc/am107_iinit.html
	MPI_Init(NULL, NULL);
	
	return getMpiRuntimeInfo(mpiComm);
}
	

/******************** Handly constant global variable *******************/
const MPI_Runtime_Info * GLOBAL_mpiRuntimeInfo = setupMPI(MPI_COMM_WORLD);


#endif /* _MPI_SUPPORT_H */
