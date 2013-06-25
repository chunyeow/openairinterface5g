This folder contains the software for the device driver of CBMIMO1 and the physical layer signal processing.


|-- ARCH   	    	   // contains drivers for hardware
|   |-- CBMIMO1		   // contains everything related to CBMIMO1
|   |   `-- DEVICE_DRIVER  
|   |       |-- Makefile             // main Makefile used to compile the driver; usually invoked from the top level Makefile
|   |       |-- cbmimo1_device.c     // main file for device driver (initializes the card, bigphys, main variables)
|   |       |-- cbmimo1_device.h
|   |       |-- cbmimo1_dma.c
|   |       |-- cbmimo1_fileops.c    // file that processes IOCTL commands coming from the userspace control utility (USERSPACE_TOOLS/OPENAIR_RF/)
|   |       |-- cbmimo1_generate_fs4.c   // generates test signals (DEBUGGING)
|   |       |-- cbmimo1_generate_ofdm.c  // generates test signals (DEBUGGING)
|   |       |-- cbmimo1_get_frame.c      // code to grab a frame from the card
|   |       |-- cbmimo1_init.c		 // code to set up the registers of the card
|   |       |-- cbmimo1_pci.h
|   |       |-- cbmimo1_proc.c           // generates the proc entries /proc/openair1/bch_stats and /proc/openair1/openair1_state 
|   |       |-- cbmimo1_rf_cntl.c	 // controls the RF chipset on the card
|   |       |-- cbmimo1_test.c
|   |       |-- from_grlib_softconfig.h
|   |       |-- from_grlib_softregs.h
|   |       |-- defs.h
|   |       |-- extern.h
|   |       `-- vars.h
|   |-- COMMON		   // common for all hardware (mainly bigphys)
|   |-- EXPRESS_MIMO	   // contains everything related to EXPRESS MIMO
|-- DOCS		   // Documentation
|   |-- DOXYGEN		   // Automaitcally generated DOXYGEN documentation in latex and html
|   `-- PAPERS		   // papers related to openair1
|-- EMOS		   // this contains everything related to Eurecom MIMO Openair Sounder (EMOS)
|   |-- DOC		   
|   |-- DUMP		   // program that dumps channel estimate to disk using a FIFO
|   |-- GUI		   
|   |-- MATLAB		   // Matlab code to process measurements
|   |-- SIM		   // EMOS simulator (for DEBUGGING)
|   `-- UTIL
|-- MAC_INTERFACE	   // interface with MAC layer (in openair2)
|-- PHY			   // contains all signal processing related to physical layer (used in real-time AND simulation)

General remarks: every directory contains at least defs.h (for declaration of structure and functions), vars.h (for definitions of variables), and extern.h (variables are defined only once in cbmimo1_device.c (for real-time HW operation) or a top-level simulation (in SIMULATION/xxx), if variables are needed in another file, use extern.h)

|   |-- CODING		   // turbo and convolutional (de-)coders, rate-matching, crc generation (implementation of 36-212)
|   |   `-- TESTBENCH      // This contains unitary testbenches for the different channel coders/decoders
|   |-- INIT		   // variables defined in vars.h are initialzed here (memory allocation)
|   |-- LTE_ESTIMATION     // estimation for LTE
|   |-- LTE_REFSIG 	   // reference signals for LTE (sync and pilot sequences) reference signals from 36-211 
|   |-- LTE_TRANSPORT      // these are the top level routines for different transport and physical channels (for example DL-SCH, PSS) implements a subset of 36-211,36-212
|   |-- MODULATION	   // modulation and demodulation includes FFT/SC-FDMA front-end processing from 36-211
|   |-- TOOLS		   // tools like (I)FFT, vector operations, matrix multiplication, etc.
|   |   `-- FFTTEST        // unitary testbench for FFT routines
|   |-- defs.h		  // top level defs.h (includes spec_defs, impl_defs and all defs.h of subdirectories)
|   |-- extern.h
|   |-- impl_defs.h       // non-LTE openair implementation
|   |-- impl_defs_lte.h   // lte implementation
|   |-- impl_defs_top.h   // general implementaion defs
|   |-- spec_defs.h
|   |-- spec_defs_top.h
|   |-- types.h
|   `-- vars.h		  
|-- SCHED		  // schedules the different PHY functions according to the nodes role (CH=BS, MR=UE)
|   |-- defs.h
|   |-- extern.h
|   |-- phy_procedures.c         // non-LTE PHY procedures this is used in synchronized mode to schedule the slot processing
|   |-- phy_procedures_emos.c    // non-LTE PHY procedures for EMOS
|   |-- phy_procedures_emos.h
|   |-- phy_procedures_lte_eNb.c // LTE PHY procedures for eNB (from 36-213) 
|   |-- phy_procedures_lte_ue.c  // LTE PHY procedures for UE (from 36-213)
|   |-- rt_compat.h
|   |-- sched.c		  // contains the top level scheduler (only for CBMIMO1 HW)
|   |-- sched_lte.c	  // same as sched.c for LTE (only for CBMIMO1 HW)
|   `-- vars.h
|-- SIMULATION		 // contains simulation routines to test PHY
|   |-- LTE_PHY          // LTE simulation testbenches for unitary simulation of transport/physical channels
|   |   |-- pbchsim.c       // PBCH simulation testbench
|   |   |-- pdcchsim.c      // PCFICH/PHICH/PDCCH (DCI) simulation testbench
|   |   |-- dlsim.c         // PDSCH simulation testbench
|   |   |-- ulsim.c         // PUSCH simulation testbench
|   |   |-- pucchsim.c      // PUCCH simulation testbench
|   |-- LTE_PHY_L2       // LTE simulation testbench for full system simulation (PHY,MAC,RLC,RRC,PDCP)
|   `-- TOOLS
|   |-- RF
|-- USERSPACE_TOOLS     // tools that run in userspace (not kernel)
|   |-- OCTAVE
|   |   |-- AGILE_RF_TOOLS // octave tools to test agile RF  
|   |   |-- CBMIMO1_TOOLS // octave interface for CBMIMO1
|   |   |-- GPIB       // octave tools to control signal generator (used for calibration)
|   |   `-- PHY_SIM    // octave implementation of sync
|   |-- OPENAIR_RF     // tool to interface with the device driver using IOCTL
|   |-- SCOPE	       // tool to visualize data from PHY (channel estimates, IQ plots, etc.). Communicates with kernel module using shared memory.
|   `-- SENSING        // tools related to sensing


