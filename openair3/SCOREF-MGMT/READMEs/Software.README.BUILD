
SCOREF-MGMT has three build configurations which are all ready to be used
through the Makefile. These are,

1) Debug Configuration

   Definition: Debug flags are on, optimisation is off, to be 
   run inside GDB, etc.

   Command: Define make variable `BUILD_DEBUG' as 
   $ make BUILD_DEBUG=1
   and relevant binary will be created in bin/

2) Dynamic Configuration

   Definition: Optimisation flags are on, dynamic linkage, for 
   deployment (along with necessary libraries) or local use

   Command: Define make variable `BUILD_DYNAMIC' as
   $ make BUILD_DYNAMIC=1
   and binary will be created in bin/

3) Static Configuration

   Definition: Static linkage, to be sent to other people 
   and/or companies whom may not have necessary libraries 
   installed

   Command: Define make variable `BUILD_STATIC' as
   $ make BUILD_STATIC=1
   and binary will be created in bin/

For Dynamic and Static configurations the make target `strip' may be used 
to save space by removing symbols in the binary as follows,

$ make strip

For Eclipse development see README.BUILD.ECLIPSE file.

Please let me know in case of a build error sending relevant library
versions, platform, etc. information.

- Baris Demiray <baris.demiray@eurecom.fr>
