/**
 * Rodney Beede
 * 2011-04-15
 *
 * http://livedocs.adobe.com/flex/3/html/help.html?content=ByteArrays_3.html
 *
 * http://www.winzip.com/aes_info.htm#file-format1
 *
 * This version also handles architectures that are big-endian (MSB) instead of LSB.
 *
*/


#include <fstream>
#include <iostream>
#include <stdint.h>  // adds support for unit8_t
#include <iomanip>

using namespace std;

// don't use unsigned char since the standard doesn't enforce a maximum range on the char type to be 8 bits
// even though the underlying compiler is probably using unsigned char anyway
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



int main (const int argc, const char * const argv[]) {
	ifstream zipfileStream;
	
	zipfileStream.open(argv[1], ios::in | ios::binary);
	
	
	zipfileStream.seekg(0, ios::end);
	
	const streampos zipfileByteSize = zipfileStream.tellg();
	
	cout << "Size of zip is " << zipfileByteSize << " bytes" << endl;
	
	zipfileStream.seekg (0, ios::beg);
	
	
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
			cerr << "Not a ZIP file!" << endl;
			return 255;
		} else {
			cout << "Fixing little endian to big endian" << endl;
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
	
	// Read in the extra field data (which may need adjusted from LSB to MSB later)
	zipfileStream.read((char *)(&header.extraField), header.extraFieldLength);
	
	cout << "File position is now " << zipfileStream.tellg() << endl;
	cout << endl;
	

	cout << "File Header Signature:\t0x" << setfill('0') << setw(8) << hex << header.fileHeaderSignature << endl;  //OUTPUT in hex
	cout << dec;
	cout << "Required Version:\t" << header.requiredVersion << endl;
	cout << "General Purpose Bit Flag:\t" << header.generalPurposeBitFlag << endl;
	cout << "Compression Method:\t" << header.compressionMethod << endl;
	// make the MS-DOS time and date into human readable
	//	http://msdn.microsoft.com/en-us/library/ms724274(v=vs.85).aspx
	//	http://cheeso.members.winisp.net/DotNetZipHelp/html/7f2034dc-5006-98f2-fc48-501330e5fb7d.htm  explains why seconds is * 2
	cout << "Last Modified File Time (MS-DOS precision) [Unformmatted]:\t" << header.lastModifiedFileTime << endl;
	// Note that LSB is bit number 0, MSB is bit number 15
	const int seconds =	(0x001F & header.lastModifiedFileTime) * 2;  //MS-DOS only has 2 second precision
	const int minutes =	(0x07E0 & header.lastModifiedFileTime) >> 5;
	const int hours =	(0xF800 & header.lastModifiedFileTime) >> 11;
	const int day =	(0x001F & header.lastModifiedFileDate);
	const int month =	(0x01E0 & header.lastModifiedFileDate) >> 5;
	const int year =	((0xFE00 & header.lastModifiedFileDate) >> 9) + 1980;  // 1980 is offset
	cout << setfill('0');
	cout << "Last Modified File Time (MS-DOS precision):\t" << setw(2) << hours << ":" << setw(2) << minutes << ":" << setw(2) << seconds << endl;
	cout << "Last Modified File Date (MS-DOS precision):\t" << setw(2) << year << "-" << setw(2) << month << "-" << setw(2) << day << endl;
	cout << dec;
	cout << "CRC-32:\t" << header.crc32 << endl;
	cout << "Compressed Size:\t" << header.compressedSize << endl;
	cout << "Uncompressed Size:\t" << header.uncompressedSize << endl;
	cout << "File Name Length\t" << header.fileNameLength << endl;
	cout << "Extra Field Length\t" << header.extraFieldLength << endl;
	cout << endl;
	cout << "File Name:\t|||" << header.fileName << "|||" << endl;
	cout << endl;
	
	if(header.extraFieldLength > 0 && 99 == header.compressionMethod) {  // 99 means AES
		const AES_ExtraDataField * aesExtraDataField = (AES_ExtraDataField *) &header.extraField;
		
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
			cerr << "Extra data field is not AES!" << endl;
			return 255;
		} else {
			cout << "AES_ExtraDataField->headerID:\t0x" << setfill('0') << setw(4) << hex << aesExtraDataField->headerID << endl;
			cout << dec;
		}
		
		cout << "AES_ExtraDataField->dataSize:\t" << aesExtraDataField->dataSize << endl;
		cout << "AES_ExtraDataField->zipVendorVersion:\t" << aesExtraDataField->zipVendorVersion << endl;
		cout << "AES_ExtraDataField->vendorId:\t" << aesExtraDataField->vendorId[0] << aesExtraDataField->vendorId[1] << endl;  // vendorId isn't null terminated
		cout << "AES_ExtraDataField->aesEncryptionStrengthMode:\t" << (int) aesExtraDataField->aesEncryptionStrengthMode << endl;  // uint8_t gets treated like char otherwise
		cout << "AES_ExtraDataField->actualCompressionMethod:\t" << aesExtraDataField->actualCompressionMethod << endl;
		cout << endl;

		// Parse the AES "Encrypted file storage format"
		
		int saltLengthInBytes = -1;
		if(0x01 == aesExtraDataField->aesEncryptionStrengthMode) {
			// 128 bit key type which uses a 8 byte salt
			saltLengthInBytes = 8;
		} else if(0x02 == aesExtraDataField->aesEncryptionStrengthMode) {
			// 192 bit key type which uses a 12 byte salt
			saltLengthInBytes = 12;
		} else if(0x03 == aesExtraDataField->aesEncryptionStrengthMode) {
			// 256 bit key type which uses a 16 byte salt
			saltLengthInBytes = 16;
		} else {
			cerr << "aesExtraDataField->aesEncryptionStrengthMode has unknown mode" << endl;
			return 255;
		}
		
		byte salt[saltLengthInBytes];
		
		zipfileStream.read((char *)(&salt), saltLengthInBytes);
		
		// since salt is an array no need to adjust for little/big endian
		
		
		byte passwordVerification[2];
		
		zipfileStream.read((char *)(&passwordVerification), 2);
		
		// since passwordVerification is an array no need to adjust for little/big endian
		
		// next is encrypted data which has length of header.compressedSize
		// we don't bother with this
		
		// next is authentication code which is 10 bytes
		// we don't bother with this
		
		
		cout << "File Salt:\t0x";
		cout << setfill('0') << hex;
		for(int i = 0; i < saltLengthInBytes; i++) {
			cout << setw(2) << (int) salt[i];  // convert to int so value isn't treated like char
		}
		cout << endl;
		
		
		// There is a 1 in 65,536 chance that an incorrect password will yield a matching verification value
		cout << "Password Verification:\t";  
		cout << "0x" << setw(2) << (int) passwordVerification[0];  // convert to int so value isn't treated like char
		cout << " ";
		cout << "0x" << setw(2) << (int) passwordVerification[1];  // convert to int so value isn't treated like char
		cout << endl;
		
		cout << endl;
		
		cout << dec;
	}


	cout << "File position is now " << zipfileStream.tellg() << hex << " (0x" << zipfileStream.tellg() << ")" << endl;
	cout << endl;
	cout << dec;
	
	
	zipfileStream.close();
}
