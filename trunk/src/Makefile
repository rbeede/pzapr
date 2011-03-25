CXXFLAGS=-DMPICH_IGNORE_CXX_SEEK


all: 
	mpicxx -Wall ${CXXFLAGS} *.cpp -o pzapr.bin
	ls -la --full-time --color
	
clean:
	rm -f *.o
	rm -f *.bin