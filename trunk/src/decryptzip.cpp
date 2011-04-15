#include "decryptzip.h"  // Only include, all others should be in the .h file
#define KEYBITS 256

using namespace std;

extern Logger * logger;

struct HeaderForFileInZip {
	uint32_t fileHeaderSignature;
	uint16_t requiredVersion;
	uint16_t generalPurposeBitFlag;
	uint16_t compressionMethod;
	uint16_t lastModifiedFileTime;
	uint16_t lastModifiedFileDate;
	uint32_t crc32;
	uint32_t compressedSize;
	uint32_t uncompressedSize;
	uint16_t fileNameLength;  // bytes
	uint16_t extraFieldLength;  // bytes
	// here (after extraFieldLength, before fileName) marks 30 bytes of the header thus far
	char fileName[65537];  // 2^16 = 65,536 + 1 for null char = 65,537 from fileNameLength
	byte extraField[65536];  // 2^16 = 65,536 from extraFieldLength
} __attribute__ ((packed));  // packed attribute allows us to read straight from disk with padding worries


struct AES_ExtraDataField {
	uint16_t headerID;  // should be 0x9901
	uint16_t dataSize;  // currently 7 for the next 7 bytes of this extra data field, but may change later
	uint16_t zipVendorVersion;
	char vendorId[2];  // 2-character vendor ID (not null terminated string)
	uint8_t aesEncryptionStrengthMode;
	uint16_t actualCompressionMethod;
} __attribute__ ((packed));  // packed attribute allows us to read straight from disk with padding worries



struct verifier_data initDecryptEngine(const char * const zipFilePathname) {

	ifstream zipfileStream;
	verifier_data verifier_data_object;		
	
	zipfileStream.open(zipFilePathname, ios_base::in | ios_base::binary);
	
	
	zipfileStream.seekg(0, ios_base::end);
	
	const streampos zipfileByteSize = zipfileStream.tellg();
	

	zipfileStream.seekg (0, ios::beg);
	
	
	/* Note that zip files store values in LSB order */
	
	// Read the first file header up to and including extraFieldLength
	HeaderForFileInZip header;
	
	zipfileStream.read((char*)&header, 30);
	
	// Read in the filename
	zipfileStream.read((char *)(&header.fileName), header.fileNameLength);
	// Enforce null termination
	header.fileName[header.fileNameLength] = '\0';
	
	// Read in the extra field data
	zipfileStream.read((char *)(&header.extraField), header.extraFieldLength);
	
	if(header.extraFieldLength > 0 && 99 == header.compressionMethod) {  // 99 means AES
		const AES_ExtraDataField * aesExtraDataField = (AES_ExtraDataField *) &header.extraField;
		
		if(0x9901 != aesExtraDataField->headerID) {
			cerr << "Extra data field is not AES!" << endl;
			//return 255;
		} else {
		}
		

		// Parse the AES "Encrypted file storage format"
		int saltLengthInBytes = -1;
		verifier_data_object.mode = -1;
		if(0x01 == aesExtraDataField->aesEncryptionStrengthMode) {
			// 128 bit key type which uses a 8 byte salt
			saltLengthInBytes = 8;
			verifier_data_object.mode = 1;
		} else if(0x02 == aesExtraDataField->aesEncryptionStrengthMode) {
			// 192 bit key type which uses a 12 byte salt
			saltLengthInBytes = 12;
			verifier_data_object.mode = 2;
		} else if(0x03 == aesExtraDataField->aesEncryptionStrengthMode) {
			// 256 bit key type which uses a 16 byte salt
			saltLengthInBytes = 16;
			verifier_data_object.mode = 3;
		} else {
			cerr << "aesExtraDataField->aesEncryptionStrengthMode has unknown mode" << endl;
			//return 255;
		}
		
		//byte salt[saltLengthInBytes];
		
		zipfileStream.read((char *)(&verifier_data_object.salt), saltLengthInBytes);
		
		
		//byte passwordVerification[2];
		
		zipfileStream.read((char *)(&verifier_data_object.passwordVerification), 2);
		
		// next is encrypted data which has length of header.compressedSize
		// we don't bother with this
		
		// next is authentication code which is 10 bytes
		// we don't bother with this
		
		

		// There is a 1 in 65,536 chance that an incorrect password will yield a matching verification value

	}


	
	zipfileStream.close();
	return verifier_data_object;
}



bool attemptPassword(struct verifier_data verifier_data_object,const std::string password) {

	fcrypt_ctx  zcx;
        byte tmp_buf[2];
        fcrypt_init(verifier_data_object.mode, (const unsigned char*)password.c_str(), (unsigned int)password.length(), verifier_data_object.salt, tmp_buf, &zcx);	//find out the value of password verifier with the given password

        if(memcmp(verifier_data_object.passwordVerification,tmp_buf,2))
        {
        	return false;	//Password dint match
        }
        else
                return true; //Password matched

}
