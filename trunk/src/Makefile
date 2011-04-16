# You MUST compile with 32-bit mode (g++ -m32) because the Gladman AES library has a bug where it doesn't work
#	on 64-bit compiled mode.  See doc folder for more information.

# Tested with OpenMPI with Ubuntu 10.10 x64 and x86

OMPI_CXXFLAGS=-DMPICH_IGNORE_CXX_SEEK -m32 -Wall


all: 
	mpicxx Gladman/*.cpp *.cpp -o pzapr.bin
	ls -la --full-time --color
	
clean:
	rm -f *.o
	rm -f *.bin