# You MUST compile with 32-bit mode (g++ -m32) because the Gladman AES library has a bug where it doesn't work
#	on 64-bit compiled mode.  See doc folder for more information.

# Tested with OpenMPI 1.4.3 running on Ubuntu 10.10 x64 and x86
#
# Your OpenMPI install must have been done after you added multilib support for 32-bit if you have Ubuntu x64
#
# Your OpenMPI install must also have been built in *32-BIT* mode, you may want separate installs for 32 & 64 bit
#
#	See http://www.open-mpi.org/community/lists/users/2006/04/0992.php
#
#	./configure --prefix=/opt/openmpi-1.4.3_32-bit CFLAGS=-m32 CXXFLAGS=-m32 FFLAGS=-m32 FCFLAGS=-m32
#
#	You'll need to put /opt/openmpi-1.4.3_32-bit/bin/  in your system path too when you make all
#
#	When you run you may get errors about missing libraries unless you use /opt/openmpi-1.4.3_32-bit/bin/mpiexec


all: 
	mpicxx -g -m32 -Wall Gladman/*.cpp *.cpp -o pzapr.bin
	
	ls -la --full-time --color
	
clean:
	rm -f *.o
	rm -f *.bin