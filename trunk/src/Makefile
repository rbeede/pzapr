CXXFLAGS=-DMPICH_IGNORE_CXX_SEEK


all: 
	mpicxx -Wall ${CXXFLAGS} *.cpp -o `basename "$(CURDIR)"`.bin
	ls -la --full-time --color
	
clean:
	rm -f *.o
	rm -f *.bin