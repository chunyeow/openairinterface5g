#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
//#include <errno.h>
#include <math.h>
#include <signal.h>
#include <strings.h>

#ifdef PLATON
#include "daq.h"

int
loadFPGA2 (unsigned int daq_fd, const char *fname)
{
  int             i, j;
  int             bitf;
  char           *map;
  unsigned char  *fpga_buf;
  int             fpga_size;
  int             pos = 0;
  struct stat     statbuf;
  unsigned int    bd;
  char            line[1024];

  printf ("Loading %s\n", fname);

  if ((bitf = open (fname, O_RDONLY)) < 0) {
    printf ("Could not open %s\n", fname);
    return -1;
  }

  if (fstat (bitf, &statbuf) == -1) {
    perror ("fstat");
    exit (1);
  }
  fpga_size = statbuf.st_size;

  printf ("Allocating %d bytes for FPGA config\n", fpga_size);

  /* We need 4 bytes to put the size of the buffer at the beginning */
  if ((fpga_buf = (unsigned char *) malloc (fpga_size)) == NULL) {
    fprintf (stderr, "can't malloc %d bytes\n", fpga_size + 4);
    exit (1);
  }

  map = mmap (0, fpga_size, PROT_READ, MAP_PRIVATE, bitf, 0);
  if (map == (caddr_t) - 1) {
    perror ("mmap failed");
    exit (-1);
  }

  j = 0;
  i = 7;                        /* skip first seven lines */

  while (i) {

    if (map[j++] == '\n')
      i--;
    line[j - 1] = map[j - 1];

  }
  line[j] = 0;
  puts (line);

  /* As there is some space left in fpga_buf[], put the size of the buffer
     in the 4 first bytes. */
  fpga_buf[0] = fpga_size & 0x000000FF;
  fpga_buf[1] = (fpga_size & 0x0000FF00) >> 8;
  fpga_buf[2] = (fpga_size & 0x00FF0000) >> 16;
  fpga_buf[3] = (fpga_size & 0xFF000000) >> 24;
  pos = 4;                      /* Protect the integer value  */
  /* Now fills the buffer. The method is ridiculous (we use 8 times the
     size needed in the fpga_buf), but works. */

  while (j < fpga_size) {       /* size must be 1751840 for XCV300 */
    switch (map[j++]) {         /* The bit */
        case '0':
          fpga_buf[pos++] = 0;
          break;
        case '1':
          fpga_buf[pos++] = 1;
          break;
        case '\n':
        case '\r':
          continue;
        default:
          printf ("Error in file format");
          return -1;
    }
  }

  close (bitf);
  munmap (map, fpga_size);

  printf ("%d bits read", pos - 4);
  *((int *) fpga_buf) = pos - 4;

  printf ("FPGA RESET");

  if (ioctl (daq_fd, DAQ_FPGA_RESET, 0) == -1) {
    perror ("ioctl");
  }
  usleep (100000);
  printf ("FPGA PROGRAMMING\n");
  if (ioctl (daq_fd, DAQ_FPGA_PROGRAM, fpga_buf) == -1) {
    perror ("ioctl");
  }
  free (fpga_buf);
  return 0;
}

int
resetFPGA2 (unsigned int daq_fd)
{

  printf ("Resetting FPGA, ioctl=%x\n", DAQ_FPGA_RESET);
  if (ioctl (daq_fd, DAQ_FPGA_RESET) == -1)
    perror ("ioctl");

  return 0;
}
#endif PLATON
