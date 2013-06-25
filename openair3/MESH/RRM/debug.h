/*!
*******************************************************************************

\file       debug.h

\brief      Fichier d'entete contenant les declarations des types, des defines ,
            et des fonctions relatives au debug .

\author     BURLOT Pascal

\date       29/08/08

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/
/*
 *
 *   Si le flag DEBUG est defini, la macro ASSERT() ne fait rien et la
 *   macro CALL() se contente d'appeler la fonction sans tester le code d'erreur.
 */

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

///< Check si le retour de fonction est egale à -1  alors abort()
#define CALL(f) \
                                                                    \
  if (((int)(f)) == -1)                                             \
  {                                                                 \
    int err = errno;                                                \
    char *strerr = strerror(err);                                   \
                                                                    \
    fprintf(stderr,                                                 \
            "CALL(%s): Error at line %u, file %s: %s (errno=%d)\n", \
            #f, __LINE__, __FILE__,                                 \
            strerr == NULL ? "Bad error number" : strerr,           \
            err);                                                   \
    abort();                                                        \
  }

///< Check si le pointeur est NULL alors abort()
#define PNULL(pp) \
                                                                    \
  if ( (pp) == NULL)                                                \
  {                                                                 \
    fprintf(stderr,                                                 \
            "PNULL(%s): Error at line %u, file %s: null pointer \n",\
            #pp, __LINE__, __FILE__ );                              \
    abort();                                                        \
  }
  
#define FOPEN(pp,arg)                                               \
  pp=fopen arg ;                                                    \
  if ( (pp) == NULL)                                                \
  {                                                                 \
    int err = errno;                                                \
    char *strerr = strerror(err);                                   \
                                                                    \
    fprintf(stderr,                                                 \
            "%s=fopen%s: \n\tError at line %u, file %s: %s (errno=%d)\n",\
            #pp, #arg, __LINE__, __FILE__ ,                         \
            strerr == NULL ? "Bad error number" : strerr,           \
            err);                                                   \
    abort();                                                        \
  }

#define DBG_FPRINT(msg)    fprintf msg
#define DBG_PRINT(msg)     printf msg

#define ASSERT(cc)                                 \
                                                   \
  if (!(cc))                                       \
  {                                                \
    fprintf(stderr,                                \
            "ASSERT(%s): at line %u, file %s\n",   \
            #cc, __LINE__, __FILE__);              \
    abort();                                       \
  }

#define WARNING(cc)                                \
                                                   \
  if ((cc))                                       \
  {                                                \
    fprintf(stderr,                                \
            "WARNING(%s): at line %u, file %s\n",  \
            #cc, __LINE__, __FILE__);              \
  }
#else

#define CALL(f) (f)
#define ASSERT(cc)
#define WARNING(cc) 
#define PNULL(pp)
#define DBG_FPRINT(msg)   
#define DBG_PRINT(msg) 
#define FOPEN(pp,arg)   pp=fopen arg

#endif
