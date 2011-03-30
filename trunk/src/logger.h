/*
 * Rodney Beede
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

		Logger(const string);
		
		void log(const char * msg);
		
		void log(const string str);
		
		
		~Logger();
		
	private:
		ofstream myfile;
};


#endif  /* _LOGGER_H */

