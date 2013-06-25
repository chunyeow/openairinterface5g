
// Maxime Guillaud - created Fri May 12 16:20:04 CEST 2006
// see http://www.gnu.org/software/octave/doc/interpreter/Dynamically-Linked-Functions.html#Dynamically-Linked-Functions
// and http://wiki.octave.org/wiki.pl?CodaTutorial
// and http://octave.sourceforge.net/coda/c58.html
// compilation: see Makefile
// Update: Wed May 23 17:25:39 CEST 2007, fifo acquisition of signal buffer (RK)


#include <octave/oct.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

extern "C" {
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/impl_defs_lte.h"
#include "ARCH/COMMON/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
}
#include "PHY/vars.h"

#define FCNNAME "oarf_send_frame"

#define TRACE 1

//extern PHY_CONFIG *PHY_config;

static bool any_bad_argument(const octave_value_list &args)
{
  octave_value v;
  if (args.length()!=3)
  {
    error(FCNNAME);
    error("syntax: oarf_send_frame(freqband,sig,nbits)\n      freqband in 0-3, sig is a 2D vector, nbits is number of bits to quantize the signal to.");
    return true;
  }

  v=args(0);
  if ((!v.is_real_scalar()) || (v.scalar_value() < 0.0) || (floor(v.scalar_value()) != v.scalar_value()) || (v.scalar_value() > 3.0))
  {
    error(FCNNAME);
    error("freqband must be 0, 1, 2, or 3.");
    return true;
  }

  v=args(1);
  printf("signal: R %d, C %d\n",v.rows(),v.columns());
  return false;

  v=args(2);
  if ((!v.is_real_scalar()) || (v.scalar_value()!=8) || (v.scalar_value()!=16)) {
    error(FCNNAME);
    error("nbits must be either 8 (CBMIMO) or 16 (ExpressMIMO)bits.");
    return true;
  }
}


DEFUN_DLD (oarf_send_frame, args, nargout,"Send frame")
{

  if (any_bad_argument(args))
       return octave_value_list();
       
  //const int freq = args(0).int_value();  
  ComplexMatrix dx = args(1).complex_matrix_value();
  
  octave_value returnvalue;
  int openair_fd,i;
  unsigned int length,aa,nbits;
  int dummy=0;

  LTE_DL_FRAME_PARMS *frame_parms = (LTE_DL_FRAME_PARMS*) malloc(sizeof(LTE_DL_FRAME_PARMS));

  TX_VARS *TX_vars = (TX_VARS*) malloc(sizeof(TX_VARS));
  TX_RX_VARS dummy_tx_rx_vars;

  unsigned int bigphys_top;
  unsigned int mem_base;

  if ((openair_fd = open("/dev/openair0", O_RDWR,0)) <0)
  {
    error(FCNNAME);
    error("Error opening /dev/openair0");
    return octave_value_list();
  }

  printf("Getting PHY_config 1 ...\n");  

  ioctl(openair_fd,openair_GET_CONFIG,frame_parms);

  dump_frame_parms(frame_parms); 

  printf("colums = %d, rows = %d\n\n\n", args(1).columns(), args(1).rows());

  if ((args(1).columns()!=NB_ANTENNAS_TX) || (args(1).rows()!=76800))
  {
    error(FCNNAME);
    error("input array must be of size (%d,%d)",76800,NB_ANTENNAS_TX);
    return octave_value_list();
  }

  nbits = args(2).scalar_value();

  //  ioctl(openair_fd,openair_STOP,(void*)&dummy);

  // version using mmap
  ioctl(openair_fd,openair_GET_VARS,(void* )&dummy_tx_rx_vars);
  ioctl(openair_fd,openair_GET_BIGPHYSTOP,(void *)&bigphys_top);
  
  if (dummy_tx_rx_vars.TX_DMA_BUFFER[0]==NULL) {
    close(openair_fd);
    error(FCNNAME);
    error("pci_buffers not allocated");
    return octave_value_list();
  }
  /*
  printf("BIGPHYS top 0x%x\n",bigphys_top);
  printf("RX_DMA_BUFFER[0] %p\n",dummy_tx_rx_vars.RX_DMA_BUFFER[0]);
  printf("TX_DMA_BUFFER[0] %p\n",dummy_tx_rx_vars.TX_DMA_BUFFER[0]);
  */
  mem_base = (unsigned int)mmap(0,
				BIGPHYS_NUMPAGES*4096,
				PROT_WRITE,
				MAP_SHARED,
				openair_fd,
				0);

  if (mem_base != -1)
    msg("MEM base= %p\n",(void*) mem_base);
  else {
    error(FCNNAME);
    error("Could not map physical memory");
    return octave_value_list();
  }



  for (aa=0;aa<NB_ANTENNAS_TX;aa++) {
    //TX_vars->TX_DMA_BUFFER[aa] = (char*) malloc(2*76800);
    TX_vars->TX_DMA_BUFFER[aa] = (char *)(mem_base + (unsigned int)dummy_tx_rx_vars.TX_DMA_BUFFER[aa]-bigphys_top);

    if (nbits==16) {
      for (i=0;i<76800;i++) {
	if (i<64)
	  printf("%d: %d,%d\n",i,(short)real(dx(i,aa)),(short)imag(dx(i,aa)));
	((short*) TX_vars->TX_DMA_BUFFER[aa])[2*i]     = (short)(real(dx(i,aa))); 
	((short*) TX_vars->TX_DMA_BUFFER[aa])[1+(2*i)] = (short)(imag(dx(i,aa)));
      }
    }
    else if (nbits==8) {
      for (i=0;i<76800;i++) {
	if (i<64)
	  printf("%d: %d,%d\n",i,char(real(dx(i,aa))),char(imag(dx(i,aa))));
	((char*) TX_vars->TX_DMA_BUFFER[aa])[2*i]     = char(real(dx(i,aa))); 
	((char*) TX_vars->TX_DMA_BUFFER[aa])[1+(2*i)] = char(imag(dx(i,aa)));
      }
    }
    else {
      error(FCNNAME);
      error("nbits has to be 8 or 16!");
    }
  }

  ioctl(openair_fd,openair_START_TX_SIG,(void *)TX_vars);

  close(openair_fd);

  munmap((void*)mem_base, BIGPHYS_NUMPAGES*4096);

  /*
  for (aa=0;aa<NB_ANTENNAS_TX;aa++)
    free(TX_vars->TX_DMA_BUFFER[aa]);
  */

  free(TX_vars);
  free(frame_parms);

  return octave_value (dx);
}


