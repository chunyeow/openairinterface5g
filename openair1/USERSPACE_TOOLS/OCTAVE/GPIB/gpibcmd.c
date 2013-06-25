/* Code of gpibcmd.c */
//
// BrettFrymire
// 04/07/2004
// Version 0.9
//Compile like this
//gcc gpibcmd.c -o test -lgpib

 

#include <stdio.h>
#include <stdlib.h>
#include "/usr/include/gpib/ib.h"
#include <strings.h>

/* Forward declares */
void check_args(int argc);


/* Type declarations */



void check_args(int argc)
{
  if (argc != 3) {
    printf("\nUsage: program Gpibaddress Command\n");
    exit(EXIT_FAILURE);
  }
}


int main(int argc, char *argv[])
{
  check_args(argc);
  unsigned short addlist[2] = {atoi(argv[1]), NOADDR}; // list to enable inst 
  //Reset Card 0 (i.e:gpib0)
  SendIFC(0);

  // ENABLE all on GPIB bus
  EnableRemote(0, addlist);
  // If error, Bye!
  if (ibsta & ERR)
  {
    printf("Instrument enable failed!\n");
    exit(1);
  }

  // Send (BoardID, GPIBAddr, Command, LengthCommand, Endline)
  // typical command:
  // Send(0, 20, "*RST", 4l, NLend);

  Send(0, atoi(argv[1]), argv[2], strlen(argv[2]), NLend);
}


