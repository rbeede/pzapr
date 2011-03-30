#include "logger.h"

void Logger::log(const char * msg) {
	time_t rawtime;
	time (&rawtime);
	tm * ptm;
	ptm = gmtime (&rawtime);
	
	myfile << (ptm->tm_year + 1900);
	myfile << "-" << setw(2) << setfill('0');
	myfile << (ptm->tm_mon + 1);
	myfile << "-" << setw(2) << setfill('0');
	myfile << (ptm->tm_mday);
	myfile << "_" << setw(2) << setfill('0');
	myfile << (ptm->tm_hour);
	myfile << ":" << setw(2) << setfill('0');
	myfile << (ptm->tm_min);
	myfile << ":" << setw(2) << setfill('0');
	myfile << (ptm->tm_sec);
	myfile << " +00:00";
	myfile << "  ";
	
	myfile << msg << endl;;
}
		
void Logger::log(const string str) {
	log(str.c_str());
}
		
		
Logger::~Logger() { myfile.close(); }


Logger::Logger(const string logFilePathname)
	: logFilePathname(logFilePathname)
{
	// Setup the log file
	
	myfile.open(logFilePathname.c_str(), ios::out);
}

