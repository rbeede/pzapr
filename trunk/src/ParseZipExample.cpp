#include <fstream>
#include <iostream>
#include <stdint.h>  // adds support for unit8_t

using namespace std;

// don't use unsigned char since the standard doesn't enforce a maximum range on the char type to be 8 bits
// even though the underlying compiler is probably using unsigned char anyway
typedef uint8_t byte;  


struct FileInZipHeader {
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


int main (const int argc, const char * const argv[]) {
	ifstream zipfileStream;
	
	zipfileStream.open(argv[1], ios_base::in | ios_base::binary);
	
	
	zipfileStream.seekg(0, ios_base::end);
	
	const streampos zipfileByteSize = zipfileStream.tellg();
	
	cout << "Size of zip is " << zipfileByteSize << " bytes" << endl;
	
	zipfileStream.seekg (0, ios::beg);
	
	
	/* Note that zip files store values in LSB order */
	
	// Read the first file header up to and including extraFieldLength
	FileInZipHeader header;
	
	zipfileStream.read((char*)&header, 30);
	
	// Read in the filename
	zipfileStream.read((char *)(&header.fileName), header.fileNameLength);
	// Enforce null termination
	header.fileName[header.fileNameLength] = '\0';
	
	// Read in the extra field data
	zipfileStream.read((char *)(&header.extraField), header.extraFieldLength);
	
	cout << "File position is now " << zipfileStream.tellg() << endl;
	cout << endl;
	
//TODO convert to hex output with 4 bytes
	cout << "File Header Signature:\t" << header.fileHeaderSignature << endl;
	cout << "Required Version:\t" << header.requiredVersion << endl;
	
	cout << "File Name Length\t" << header.fileNameLength << endl;
	cout << "Extra Field Length\t" << header.extraFieldLength << endl;
	
	
	
	zipfileStream.close();
}