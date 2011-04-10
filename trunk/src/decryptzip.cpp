#include "decryptzip.h"  // Only include, all others should be in the .h file

#define KEYBITS 256



extern Logger * logger;



// Global variables that should only be used by decryptzip

char * zipfileBytes;

long sizeofzip;



void initDecryptEngine(const char * const zipFilePathname) {

	//read in zip file and store content in global variable	

	logger->log("Reading zip file at " + to_string(zipFilePathname));

	

	std::ifstream zipfileStream;

	zipfileStream.open(zipFilePathname, ios::in);

	

	//get byte size of zip file

	if(zipfileStream.is_open())

	{

		//long sizeofzip;

		zipfileStream.seekg(0,std::ios::end);

		sizeofzip = zipfileStream.tellg();

		zipfileStream.seekg(0,std::ios::beg);



		//zipfileBytes = new unsigned char[SIZE OF ZIP];

		zipfileBytes = new char[sizeofzip];

	

		//read in zip file into memory

		zipfileStream.read(zipfileBytes,sizeofzip);



		// close the file

		zipfileStream.close();

	}

	else {

		logger->log("Unable to open the zip file");

		//TODO mpi abort call

	}

}



void decryptBlock(unsigned char ciphertext[],const unsigned char* key)

{

        int nrounds;

	unsigned long *rk;

	unsigned char plaintext[16];

	rk = (unsigned long*)malloc(sizeof(char)*4*60);

        nrounds = rijndaelSetupDecrypt(rk, key, KEYBITS);

        rijndaelDecrypt(rk, nrounds, ciphertext, plaintext);

	printf("%s\n",plaintext);

}



void getBlock(unsigned char ciphertext[16])

{

	//unsigned char ciphertext[16];

	static int cnt = 0;

	for(int i=0;i<16;i++)

	{

		if(cnt<sizeofzip)

		{

			ciphertext[i] = zipfileBytes[cnt];

			cnt++;

		}

		else

		{

			ciphertext[i] = '\0';

			break;

		}

	}

	//return ciphertext;

}



bool attemptPassword(const char * const zipFilePathname,const std::string password) {

	//unsigned char ciphertext[16];

	//getBlock(ciphertext);

	//decryptBlock(ciphertext,(const unsigned char*)password.c_str());

	FILE *inf;

	unsigned char tmp_buf1[16], tmp_buf2[16], salt[16];

	char *fname,*cp;

	fcrypt_ctx  zcx[1];

	int len, flen, err = 0;

	unsigned char mode;

	int a;



	len = password.length();

	mode = (len < 32 ? 1 : len < 48 ? 2 : 3);



    	fname = (char*)malloc(strlen(zipFilePathname) + 5);

	strcpy(fname, zipFilePathname);

	inf = fopen(fname, "rb");



	if((cp = strrchr(fname, '.')) && strcmp(cp, ".zip") == 0) //this was .enc instead of .zip in original file

    	{

       		*cp = 0;

	        mode |= 4;  /* signal decryption */

        }

    	else                        /* add ".enc" to file name to mark the  */

	        strcat(fname, ".zip");  /* the file as an encrypted one         */



	fseek(inf, 0, SEEK_END);

    	flen = ftell(inf);

    	fseek(inf, 0, SEEK_SET);

    	mode &= 3;



    	/* recover the password salt     */

	fread(salt, sizeof(unsigned char), SALT_LENGTH(mode), inf); flen -= SALT_LENGTH(mode);



    	/* initialise encryption and authentication */

    	fcrypt_init(mode, (const unsigned char*)password.c_str(), (unsigned int)password.length(), salt, tmp_buf2, zcx);



    	/* recover the password verifier (if used)  */

    	fread(tmp_buf1, sizeof(unsigned char), PWD_VER_LENGTH, inf); flen -= PWD_VER_LENGTH;



    	/* check password verifier  */

    	if(memcmp(tmp_buf1, tmp_buf2, PWD_VER_LENGTH))

    	{

		//printf("\n%s\n%s\n%s\n%d\n",salt,tmp_buf1,tmp_buf2,PWD_VER_LENGTH);

        	a = 0;

		return false;

    	}

	else

		a = 1;

		printf("\n\n%d\n",a); //a=0, password dint work, a=1 password worked

	return true;

}


