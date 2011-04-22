/*
 * Rodney Beede
 *
 * 2011-04-05
 *
*/

#ifndef _LOGGER_H
#define _LOGGER_H


#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>

using namespace std;


class Logger {
	public:
		const string logFilePathname;

		// Constructor
		// Opens log file located at logFilePathname for writing (truncates any existing file)
		Logger(const string logFilePathname);
		
		// Records the message to the log preceded by a timestamp and with a newline added for you to the end
		void log(const char * msg);
		
		// Convience method to log(const char * msg)
		void log(const string str);
		
		// Destructor
		// Closes log file
		~Logger();
		
	private:
		ofstream myfile;
};


#endif  /* _LOGGER_H */

