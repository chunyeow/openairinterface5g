#ifndef __PHY_SPEC_DEFS_TOP_H__
#define __PHY_SPEC_DEFS_TOP_H__

#include "types.h"



/*! \brief Extension Type */
typedef enum {
   CYCLIC_PREFIX,
   CYCLIC_SUFFIX,
   ZEROS,
   NONE
 } Extension_t;


/// mod_sym_t is the type of txdataF
#ifdef IFFT_FPGA
typedef unsigned char mod_sym_t; 
#else
typedef int mod_sym_t;
#endif //IFFT_FPGA


#endif /*__PHY_SPEC_DEFS_TOP_H__ */ 















