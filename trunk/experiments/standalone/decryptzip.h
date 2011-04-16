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
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <iomanip>

#include "stringutils.h"
#include "logger.h"
#include "Gladman/rijndael.h"
#include "Gladman/fileenc.h"

typedef uint8_t byte;  

struct verifier_data{
        int mode;
        byte salt[16];
        byte passwordVerification[2];
};

void initDecryptEngine(const char * const zipFilePathname);

bool attemptPassword(const std::string password);


#endif /* _DECRYPTZIP_H */
