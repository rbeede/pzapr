#include "decryptzip.h"  // Only include, all others should be in the .h file

using namespace std;

struct verifier_data verifier_data_object;

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



void initDecryptEngine(const char * const zipFilePathname) {

	ifstream zipfileStream;

	
	zipfileStream.open(zipFilePathname, ios::in | ios::binary);
	
	
	zipfileStream.seekg(0, ios::end);
	
	const streampos zipfileByteSize = zipfileStream.tellg();
	
	zipfileStream.seekg(0, ios_base::beg);
		
	if(zipfileStream.fail()) {
		logger->log("FATAL ERROR:  Failed to rewind zip file stream!");
	}
	
	/* Note that zip files store values in LSB order */
	
	// Read the first file header up to and including extraFieldLength
	HeaderForFileInZip header;
	
	zipfileStream.read((char*)&header, 30);
	
	
	// Endian check
	const bool littleEndian = (0x04034b50 == header.fileHeaderSignature);
	if(!littleEndian) {
		// Either not a zip file or this architecture is MSB (big endian) so we need to check
		header.fileHeaderSignature = FIX_INT(header.fileHeaderSignature);
		
		if(0x04034b50 != header.fileHeaderSignature) {
			logger->log("Not a ZIP file!");
			return;
		} else {
			logger->log("Fixing little endian to big endian");
		}
		
		// Now to correct the rest of it
		header.requiredVersion = FIX_SHORT(header.requiredVersion);
		header.generalPurposeBitFlag = FIX_SHORT(header.generalPurposeBitFlag);
		header.compressionMethod = FIX_SHORT(header.compressionMethod);
		header.lastModifiedFileTime = FIX_SHORT(header.lastModifiedFileTime);
		header.lastModifiedFileDate = FIX_SHORT(header.lastModifiedFileDate);
		header.crc32 = FIX_INT(header.crc32);
		header.compressedSize = FIX_INT(header.compressedSize);
		header.uncompressedSize = FIX_INT(header.uncompressedSize);
		header.fileNameLength = FIX_SHORT(header.fileNameLength);
		header.extraFieldLength = FIX_SHORT(header.extraFieldLength);
	}
	
	
	// Read in the filename
	zipfileStream.read((char *)(&header.fileName), header.fileNameLength);
	// Enforce null termination
	header.fileName[header.fileNameLength] = '\0';
	
	// Read in the extra field data
	zipfileStream.read((char *)(&header.extraField), header.extraFieldLength);
	
	if(header.extraFieldLength > 0 && 99 == header.compressionMethod) {  // 99 means AES
		AES_ExtraDataField * aesExtraDataField = (AES_ExtraDataField *) &header.extraField;
		
		if(!littleEndian) {
			// Need to adjust the header to big endian
			aesExtraDataField->headerID = FIX_SHORT(aesExtraDataField->headerID);
			aesExtraDataField->dataSize = FIX_SHORT(aesExtraDataField->dataSize);
			aesExtraDataField->zipVendorVersion = FIX_SHORT(aesExtraDataField->zipVendorVersion);
			// vendorId is a char array so no need to fix
			// aesEncryptionStrengthMode is only 1 byte so nothing to fix
			aesExtraDataField->actualCompressionMethod = FIX_SHORT(aesExtraDataField->actualCompressionMethod);
		}
		
		
		if(0x9901 != aesExtraDataField->headerID) {
			logger->log("Extra data field is not AES!");
			return;
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
		 }
		
		zipfileStream.read((char *)(&verifier_data_object.salt), saltLengthInBytes);
		// since salt is an array no need to adjust for little/big endian
		
		zipfileStream.read((char *)(&(verifier_data_object.passwordVerification)), 2);
		// since passwordVerification is an array no need to adjust for little/big endian
		
		// next is encrypted data which has length of header.compressedSize
		//comprssed size is total of saltLength, password verifier, encrypted data and MAC
		verifier_data_object.fileDataSize = (int) header.compressedSize - (saltLengthInBytes + 2 + MAC_LENGTH(mode));
		verifier_data_object.fileData = new byte[verifier_data_object.fileDataSize];
		zipfileStream.read((char *)verifier_data_object.fileData, verifier_data_object.fileDataSize);
		
		// next is authentication code which is 10 bytes
		zipfileStream.read((char *)(&(verifier_data_object.authenticationcode)), MAC_LENGTH(mode));
	} else {
		logger->log("ERROR:  ZIP FILE IS NOT AES ENCRYPTED!");
	}

	zipfileStream.close();
}



bool attemptPassword(const std::string password) {
	fcrypt_ctx zcx;
	unsigned char tmp_buf[2];

	//find out the value of password verifier with the given password
	fcrypt_init(verifier_data_object.mode, (const unsigned char*)password.c_str(), (unsigned int)password.length(), verifier_data_object.salt, tmp_buf, &zcx);	

	if(memcmp(verifier_data_object.passwordVerification,tmp_buf,2)) {
		return false;	//Password did not match
	} else {
		// Make sure it isn't a false positive (1 in 65535 chance)
		
		// We do this by actually decrypting the file and checking the ZIP AES authentication code to see if it matches
		//	This is basically a checksum comparison and is pretty fast for small files.  We should only have to do this
		//	a few times and not for every possible password
		fcrypt_decrypt(verifier_data_object.fileData, verifier_data_object.fileDataSize, &zcx);
		byte authCodeFromDecrypt[MAC_LENGTH(verifier_data_object.mode)];
		fcrypt_end(authCodeFromDecrypt,&zcx);


		if(memcmp(verifier_data_object.authenticationcode, authCodeFromDecrypt, MAC_LENGTH(verifier_data_object.mode))){
			return false;   //Password matched but not MAC
		} else {
			return true; //Password matched as well as MAC
		}
	}
}
