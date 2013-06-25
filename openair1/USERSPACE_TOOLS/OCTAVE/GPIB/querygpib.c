/* Code of QueryGpib.c */
//
// BrettFrymire
// 04/07/2004
// Version 0.9
//Compile like this
//gcc test.c -o test -lgpib

#include <stdio.h>
#include <stdlib.h>
#include "ib.h"
#include <strings.h>


/* Type declarations */
char inData[19];
 

void check_args(int argc)
{
  if (argc != 3) 
  {
    printf("\nUsage: program Gpibaddress Command\n");
    exit(EXIT_FAILURE);
  }
}


int main(int argc, char *argv[])
{
  int loop;
  check_args(argc);
  for (loop=0;loop<=19-1;loop++)
  {
    inData[loop] = 0;
  }
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
  Receive(0, atoi(argv[1]), inData, 19, STOPend);
  printf("%s",inData);
}
 
