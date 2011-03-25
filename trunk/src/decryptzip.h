/*
 * Neelam
 *
*/

#ifndef _DECRYPTZIP_H
#define _DECRYPTZIP_H

#include <string>
#include <fstream>

void initDecryptEngine(const char * const zipFilePathname);

bool attemptPassword(const std::string password);


#endif /* _DECRYPTZIP_H */