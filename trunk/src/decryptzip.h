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

#include "mpi.h"

typedef uint8_t byte;  

// http://paulbourke.net/dataformats/endian/
#define SWAP_2(x) ( (((x) & 0xff) << 8) | ((unsigned short)(x) >> 8) )
#define SWAP_4(x) ( ((x) << 24) | \
         (((x) << 8) & 0x00ff0000) | \
         (((x) >> 8) & 0x0000ff00) | \
         ((x) >> 24) )
#define FIX_SHORT(x) (*(unsigned short *)&(x) = SWAP_2(*(unsigned short *)&(x)))
#define FIX_INT(x)   (*(unsigned int *)&(x)   = SWAP_4(*(unsigned int *)&(x)))
#define FIX_FLOAT(x) FIX_INT(x)


struct verifier_data{
        int mode;
        byte salt[16];
        byte passwordVerification[2];
        byte * fileData;
        byte authenticationcode[10];
        int fileDataSize;
};

void initDecryptEngine(const char * const zipFilePathname);

bool attemptPassword(const std::string password);


#endif /* _DECRYPTZIP_H */
