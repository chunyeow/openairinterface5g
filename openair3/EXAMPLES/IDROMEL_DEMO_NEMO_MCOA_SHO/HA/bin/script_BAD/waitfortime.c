#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>

#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>

#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <argp.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define MSTOUS 1000000

int main (int argc, char **argv) 
{
  fd_set fdset;
  int fdmax;
  struct timeval timeout;
  struct timeval last, intvl, now;
  struct timeval *t = NULL;
  long target_time;

  fdmax = 1;
  intvl.tv_sec = 0;
  intvl.tv_usec = 1000;

  if (argc != 2){
    printf("waitfortime time(in sec)\n");
    return -1;
  }

  target_time = atol(argv[1]);
  gettimeofday (&last, NULL);
  if (last.tv_sec >= target_time){
    return -1;
  }
  else while (last.tv_sec != target_time){
      int n;
      FD_ZERO (&fdset);
      gettimeofday (&now, NULL);
      timeout.tv_sec = last.tv_sec + intvl.tv_sec - now.tv_sec;
      timeout.tv_usec = last.tv_usec + intvl.tv_usec - now.tv_usec;

      while (timeout.tv_usec < 0)
	{
	  timeout.tv_usec += 1000000;
	  timeout.tv_sec--;
	}
      while (timeout.tv_usec >= 1000000)
	{
	  timeout.tv_usec -= 1000000;
	  timeout.tv_sec++;
	}

      if (timeout.tv_sec < 0)
	timeout.tv_sec = timeout.tv_usec = 0;

      n = select (fdmax, &fdset, NULL, NULL, &timeout);
      if (n < 0)
	{
	  perror ("select");
	  continue;
	}
      else if (n == 1)
	{
	  break;
	}
      else{
	gettimeofday (&last, NULL);
      }
    }
  return 0;
}

