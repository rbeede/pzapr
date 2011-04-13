/*

 * Neelam

 *

*/



#ifndef _DECRYPTZIP_H

#define _DECRYPTZIP_H



#include <string>

#include <fstream>

#include <iostream>

#include <malloc.h>

#include <stdlib.h>

#include <memory.h>



#include "stringutils.h"

#include "logger.h"

#include "Gladman/rijndael.h"

#include "Gladman/fileenc.h"



void initDecryptEngine(const char * const zipFilePathname);



bool attemptPassword(const char * const zipFilePathname,const std::string password);





#endif /* _DECRYPTZIP_H */
