/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
/*! \file MIH_C_Types.h
 * \brief This file defines the 802.21 types or constants defined in Std 802.21-2008 Table F1, F2, F3,
 *         F4, F5, F6, F7, F8, F9, F12, F13, F19, F20, F22, L2. Some tables may be not implemented at all.
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */
#ifndef __MIH_C_LINK_TYPES_H__
#define __MIH_C_LINK_TYPES_H__
#include <sys/types.h>
#include <linux/types.h>

#ifndef USER_MODE
# define USER_MODE
# warning "Hack USER_MODE"
#endif
//-----------------------------------------------------------------------------
#include "MIH_C_Link_Constants.h"
#include "MIH_C_bit_buffer.h"
//-----------------------------------------------------------------------------
#define MIH_C_DEBUG_SERIALIZATION 1
#define MIH_C_DEBUG_DESERIALIZATION 1
//-----------------------------------------------------------------------------
#define STR(x)        #x

/** \defgroup MIH_C_INTERFACE 802.21 interface
 *
 */


//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.1 Basic data types
//-----------------------------------------------------------------------------


//----------------------- BITMAP(size) ----------------------------------------
/*! \var typedef u_int8_t                             MIH_C_BITMAP8_T;
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A type definition for u_int8_t.
 */
typedef u_int8_t                                                              MIH_C_BITMAP8_T;
/*! \var typedef u_int16_t                             MIH_C_BITMAP16_T;
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A type definition for u_int16_t.
 */
typedef u_int16_t                                                             MIH_C_BITMAP16_T;
/*! \struct  MIH_C_BITMAP24_T
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 */
typedef struct MIH_C_BITMAP24 {u_int8_t val[3];}__attribute__((__packed__))   MIH_C_BITMAP24_T;
/*! \var typedef u_int32_t                             MIH_C_BITMAP32_T;
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A type definition for u_int32_t.
 */
typedef u_int32_t                                                             MIH_C_BITMAP32_T;
/*! \var typedef u_int64_t                             MIH_C_BITMAP64_T;
 * \brief A type definition for u_int64_t.
 */
typedef u_int64_t                                                             MIH_C_BITMAP64_T;
/*! \struct  MIH_C_BITMAP128_T
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 */
typedef struct MIH_C_BITMAP128 {u_int64_t val[2];}__attribute__((__packed__)) MIH_C_BITMAP128_T;
/*! \struct  MIH_C_BITMAP256_T
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 */
typedef struct MIH_C_BITMAP256 {u_int64_t val[4];}__attribute__((__packed__)) MIH_C_BITMAP256_T;

#ifdef MIH_C_F1_BASIC_DATA_TYPES_CODEC_C
    /*! \def TYPEDEF_BITMAP6(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_BITMAP8_T.
     */
    #define TYPEDEF_BITMAP6(DATA_TYPE_NAME)             typedef MIH_C_BITMAP8_T  DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_BITMAP8_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP8_T* dataP);\
        extern inline void MIH_C_BITMAP8_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP8_T* dataP);\
        inline unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP) {\
            return sprintf(buffP, "0x%02X", *bitmapP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {\
            MIH_C_BITMAP8_encode(bbP, bitmapP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP8_decode(bbP, bitmapP);};

    /*! \def TYPEDEF_BITMAP8(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_BITMAP8_T.
     */
    #define TYPEDEF_BITMAP8(DATA_TYPE_NAME)             typedef MIH_C_BITMAP8_T  DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_BITMAP8_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP8_T* dataP);\
        extern inline void MIH_C_BITMAP8_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP8_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP) {\
            return sprintf(buffP, "0x%02X", *bitmapP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {\
            MIH_C_BITMAP8_encode(bbP, bitmapP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP8_decode(bbP, bitmapP);};

    /*! \def TYPEDEF_BITMAP16(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_BITMAP16_T.
     */
    #define TYPEDEF_BITMAP16(DATA_TYPE_NAME)            typedef MIH_C_BITMAP16_T DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_BITMAP16_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP16_T* dataP);\
        extern inline void MIH_C_BITMAP16_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP16_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP) {\
            return sprintf(buffP, "0x%04X", *bitmapP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {\
            MIH_C_BITMAP16_encode(bbP, bitmapP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP16_decode(bbP, bitmapP);};

    /*! \def TYPEDEF_BITMAP20(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_BITMAP24_T.
     */
    #define TYPEDEF_BITMAP20(DATA_TYPE_NAME)            typedef MIH_C_BITMAP24_T DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_BITMAP24_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP24_T* dataP);\
        extern inline void MIH_C_BITMAP24_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP24_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP) {\
            return sprintf(buffP, "0x%02X%02X%02X", bitmapP->val[0], bitmapP->val[1], bitmapP->val[2]);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {\
            MIH_C_BITMAP24_encode(bbP, bitmapP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP24_decode(bbP, bitmapP);};

    /*! \def TYPEDEF_BITMAP32(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_BITMAP32_T.
     */
    #define TYPEDEF_BITMAP32(DATA_TYPE_NAME)            typedef MIH_C_BITMAP32_T DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_BITMAP32_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP32_T* dataP);\
        extern inline void MIH_C_BITMAP32_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP32_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP) {\
            return sprintf(buffP, "0x%08X", *bitmapP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {\
            MIH_C_BITMAP32_encode(bbP, bitmapP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP32_decode(bbP, bitmapP);};

    /*! \def TYPEDEF_BITMAP64(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_BITMAP64_T.
     */
    #define TYPEDEF_BITMAP64(DATA_TYPE_NAME)            typedef MIH_C_BITMAP64_T DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_BITMAP64_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP64_T* dataP);\
        extern inline void MIH_C_BITMAP64_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP64_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP) {\
            unsigned int buffer_index = 0;\
            buffer_index = sprintf(buffP, "0x%08x", (MIH_C_UNSIGNED_INT4_T)(*bitmapP>>32));\
            buffer_index = sprintf(&buffP[buffer_index], "%08x", (MIH_C_UNSIGNED_INT4_T)(*bitmapP));\
            return buffer_index;\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {\
            MIH_C_BITMAP64_encode(bbP, bitmapP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP64_decode(bbP, bitmapP);};

    /*! \def TYPEDEF_BITMAP128(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_BITMAP128_T.
     */
    #define TYPEDEF_BITMAP128(DATA_TYPE_NAME)           typedef MIH_C_BITMAP128_T DATA_TYPE_NAME  ## _T;\
        inline void MIH_C_BITMAP128_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP128_T* dataP);\
        inline void MIH_C_BITMAP128_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP128_T* dataP);\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP128_encode(bbP, bitmapP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP128_decode(bbP, bitmapP);};

    /*! \def TYPEDEF_BITMAP256(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_BITMAP256_T.
     */
    #define TYPEDEF_BITMAP256(DATA_TYPE_NAME)           typedef MIH_C_BITMAP256_T DATA_TYPE_NAME  ## _T;\
        inline void MIH_C_BITMAP256_encode(Bit_Buffer_t* bbP, MIH_C_BITMAP256_T* dataP);\
        inline void MIH_C_BITMAP256_decode(Bit_Buffer_t* bbP, MIH_C_BITMAP256_T* dataP);\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP256_encode(bbP, bitmapP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *bitmapP) {MIH_C_BITMAP256_decode(bbP, bitmapP);};

#else
    #define TYPEDEF_BITMAP6(DATA_TYPE_NAME)             typedef MIH_C_BITMAP8_T  DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);

    #define TYPEDEF_BITMAP8(DATA_TYPE_NAME)             typedef MIH_C_BITMAP8_T  DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);

    #define TYPEDEF_BITMAP16(DATA_TYPE_NAME)            typedef MIH_C_BITMAP16_T DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);

    #define TYPEDEF_BITMAP20(DATA_TYPE_NAME)            typedef MIH_C_BITMAP24_T DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);

    #define TYPEDEF_BITMAP32(DATA_TYPE_NAME)            typedef MIH_C_BITMAP32_T DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);

    #define TYPEDEF_BITMAP64(DATA_TYPE_NAME)            typedef MIH_C_BITMAP64_T DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *bitmapP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);

    #define TYPEDEF_BITMAP128(DATA_TYPE_NAME)           typedef MIH_C_BITMAP128_T DATA_TYPE_NAME  ## _T;\
        extern inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);\
        extern inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);

    #define TYPEDEF_BITMAP256(DATA_TYPE_NAME)           typedef MIH_C_BITMAP256_T DATA_TYPE_NAME  ## _T;\
        extern inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);\
        extern inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T* bitmapP);
#endif

#define BITMAP6(VAR_NAME)                                   MIH_C_BITMAP8_T   VAR_NAME;
#define BITMAP8(VAR_NAME)                                   MIH_C_BITMAP8_T   VAR_NAME;
#define BITMAP16(VAR_NAME)                                  MIH_C_BITMAP16_T  VAR_NAME;
#define BITMAP20(VAR_NAME)                                  MIH_C_BITMAP24_T  VAR_NAME;
#define BITMAP24(VAR_NAME)                                  MIH_C_BITMAP24_T  VAR_NAME;
#define BITMAP32(VAR_NAME)                                  MIH_C_BITMAP32_T  VAR_NAME;
#define BITMAP64(VAR_NAME)                                  MIH_C_BITMAP64_T  VAR_NAME;
#define BITMAP128(VAR_NAME)                                 MIH_C_BITMAP128_T VAR_NAME;
#define BITMAP256(VAR_NAME)                                 MIH_C_BITMAP256_T VAR_NAME;

//----------------------- CHOICE(DATATYPE1, DATATYPE2[,...]) ------------------
/*! \var typedef u_int8_t                             MIH_C_CHOICE_T;
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A type definition for u_int8_t.
 */
typedef u_int8_t                           MIH_C_CHOICE_T;

//----------------------- INTEGER(size) ---------------------------------------
/*! \var typedef int8_t                             MIH_C_INTEGER1_T;
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A type definition for int8_t.
 */
typedef int8_t                             MIH_C_INTEGER1_T;
/*! \var typedef int16_t                             MIH_C_INTEGER2_T;
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A type definition for int16_t.
 */
typedef int16_t                            MIH_C_INTEGER2_T;
/*! \var typedef int32_t                             MIH_C_INTEGER4_T;
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A type definition for int32_t.
 */
typedef int32_t                            MIH_C_INTEGER4_T;
/*! \var typedef int64_t                             MIH_C_INTEGER8_T;
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A type definition for int64_t.
 */
typedef int64_t                            MIH_C_INTEGER8_T;

#ifdef MIH_C_F1_BASIC_DATA_TYPES_CODEC_C
    /*! \def TYPEDEF_INTEGER1(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_INTEGER1_T.
     */
    #define TYPEDEF_INTEGER1(DATA_TYPE_NAME)            typedef MIH_C_INTEGER1_T   DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_INTEGER1_encode(Bit_Buffer_t* bbP, MIH_C_INTEGER1_T* dataP);\
        extern inline void MIH_C_INTEGER1_decode(Bit_Buffer_t* bbP, MIH_C_INTEGER1_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP) {\
            return sprintf(buffP, "0x%02X", *dataP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_INTEGER1_encode(bbP, (MIH_C_INTEGER1_T*)dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_INTEGER1_decode(bbP, (MIH_C_INTEGER1_T*)dataP);\
        };

    /*! \def TYPEDEF_INTEGER2(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_INTEGER2_T.
     */
    #define TYPEDEF_INTEGER2(DATA_TYPE_NAME)            typedef MIH_C_INTEGER2_T   DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_INTEGER2_encode(Bit_Buffer_t* bbP, MIH_C_INTEGER2_T* dataP);\
        extern inline void MIH_C_INTEGER2_decode(Bit_Buffer_t* bbP, MIH_C_INTEGER2_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP) {\
            return sprintf(buffP, "0x%04X", *dataP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_INTEGER2_encode(bbP, (MIH_C_INTEGER2_T*)dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_INTEGER2_decode(bbP, (MIH_C_INTEGER2_T*)dataP);\
        };

    /*! \def TYPEDEF_INTEGER4(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_INTEGER4_T.
     */
    #define TYPEDEF_INTEGER4(DATA_TYPE_NAME)            typedef MIH_C_INTEGER4_T   DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_INTEGER4_encode(Bit_Buffer_t* bbP, MIH_C_INTEGER4_T* dataP);\
        extern inline void MIH_C_INTEGER4_decode(Bit_Buffer_t* bbP, MIH_C_INTEGER4_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP) {\
            return sprintf(buffP, "0x%08X", (MIH_C_INTEGER4_T)*dataP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_INTEGER4_encode(bbP, (MIH_C_INTEGER4_T*)dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_INTEGER4_decode(bbP, (MIH_C_INTEGER4_T*)dataP);\
        };

    /*! \def TYPEDEF_INTEGER8(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_INTEGER48T.
     */
    #define TYPEDEF_INTEGER8(DATA_TYPE_NAME)            typedef MIH_C_INTEGER8_T   DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_INTEGER8_encode(Bit_Buffer_t* bbP, MIH_C_INTEGER8_T* dataP);\
        extern inline void MIH_C_INTEGER8_decode(Bit_Buffer_t* bbP, MIH_C_INTEGER8_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP) {\
            unsigned int buffer_index = 0;\
            buffer_index = sprintf(buffP, "0x%08x", (MIH_C_UNSIGNED_INT4_T)(*dataP>>32));\
            buffer_index = sprintf(&buffP[buffer_index], "%08x", (MIH_C_UNSIGNED_INT4_T)(*dataP));\
            return buffer_index;\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_INTEGER8_encode(bbP, (MIH_C_INTEGER8_T*)dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_INTEGER8_decode(bbP, (MIH_C_INTEGER8_T*)dataP);\
        };
#else
    #define TYPEDEF_INTEGER1(DATA_TYPE_NAME)            typedef MIH_C_INTEGER1_T   DATA_TYPE_NAME  ## _T;\
        extern unsigned int  DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP);\
        extern inline void   DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void   DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);

    #define TYPEDEF_INTEGER2(DATA_TYPE_NAME)            typedef MIH_C_INTEGER2_T   DATA_TYPE_NAME  ## _T;\
        extern unsigned int  DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP);\
        extern inline void   DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void   DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);

    #define TYPEDEF_INTEGER4(DATA_TYPE_NAME)            typedef MIH_C_INTEGER4_T   DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);

    #define TYPEDEF_INTEGER8(DATA_TYPE_NAME)            typedef MIH_C_INTEGER8_T   DATA_TYPE_NAME  ## _T;\
        extern unsigned int  DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP);\
        extern inline void   DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void   DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);
#endif

/*! \def INTEGER1(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_INTEGER1_T.
 */
#define INTEGER1(VAR_NAME)                                  MIH_C_INTEGER1_T   VAR_NAME;
/*! \def INTEGER2(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_INTEGER2_T.
 */
#define INTEGER2(VAR_NAME)                                  MIH_C_INTEGER2_T   VAR_NAME;
/*! \def INTEGER4(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_INTEGER4_T.
 */
#define INTEGER4(VAR_NAME)                                  MIH_C_INTEGER4_T   VAR_NAME;
/*! \def INTEGER8(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_INTEGER8_T.
 */
#define INTEGER8(VAR_NAME)                                  MIH_C_INTEGER8_T   VAR_NAME;


//----------------------- UNSIGNED_INT(size) ----------------------------------
/*! \var typedef int8_t                     MIH_C_UNSIGNED_INT1_T;
 * \brief A type definition for int8_t.
 */
typedef u_int8_t                           MIH_C_UNSIGNED_INT1_T;
/*! \var u_int16_t                          MIH_C_UNSIGNED_INT2_T
 * \brief A type definition for u_int16_t.
 */
typedef u_int16_t                          MIH_C_UNSIGNED_INT2_T;
/*! \var typedef u_int32_t                  MIH_C_UNSIGNED_INT4_T;
 * \brief A type definition for u_int32_t.
 */
typedef u_int32_t                          MIH_C_UNSIGNED_INT4_T;
/*! \var u_int64_t                          MIH_C_UNSIGNED_INT8_T;
 * \brief A type definition for u_int64_t.
 */
typedef u_int64_t                          MIH_C_UNSIGNED_INT8_T;

#ifdef MIH_C_F1_BASIC_DATA_TYPES_CODEC_C
    /*! \def TYPEDEF_UNSIGNED_INT1(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_UNSIGNED_INT1_T.
     */
    #define TYPEDEF_UNSIGNED_INT1(DATA_TYPE_NAME)            typedef MIH_C_UNSIGNED_INT1_T   DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_UNSIGNED_INT1_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT1_T* dataP);\
        extern inline void MIH_C_UNSIGNED_INT1_decode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT1_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP) {\
            return sprintf(buffP, "0x%02X", *dataP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_UNSIGNED_INT1_encode(bbP, dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_UNSIGNED_INT1_decode(bbP, dataP);\
        };

    /*! \def TYPEDEF_UNSIGNED_INT2(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_UNSIGNED_INT2_T.
     */
    #define TYPEDEF_UNSIGNED_INT2(DATA_TYPE_NAME)            typedef MIH_C_UNSIGNED_INT2_T   DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_UNSIGNED_INT2_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT2_T* dataP);\
        extern inline void MIH_C_UNSIGNED_INT2_decode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT2_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP) {\
            return sprintf(buffP, "0x%04X", *dataP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_UNSIGNED_INT2_encode(bbP, dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_UNSIGNED_INT2_decode(bbP, dataP);\
        };

    /*! \def TYPEDEF_UNSIGNED_INT4(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_UNSIGNED_INT4_T.
     */
    #define TYPEDEF_UNSIGNED_INT4(DATA_TYPE_NAME)            typedef MIH_C_UNSIGNED_INT4_T   DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_UNSIGNED_INT4_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT4_T* dataP);\
        extern inline void MIH_C_UNSIGNED_INT4_decode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT4_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP) {\
            return sprintf(buffP, "0x%08X", *dataP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_UNSIGNED_INT4_encode(bbP, dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_UNSIGNED_INT4_decode(bbP, dataP);\
        };

    /*! \def TYPEDEF_UNSIGNED_INT8(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a MIH_C_UNSIGNED_INT8_T.
     */
    #define TYPEDEF_UNSIGNED_INT8(DATA_TYPE_NAME)            typedef MIH_C_UNSIGNED_INT8_T   DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_UNSIGNED_INT8_encode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT8_T* dataP);\
        extern inline void MIH_C_UNSIGNED_INT8_decode(Bit_Buffer_t* bbP, MIH_C_UNSIGNED_INT8_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP) {\
            return sprintf(buffP, "0x%16X", *dataP);\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_UNSIGNED_INT8_encode(bbP, dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_UNSIGNED_INT8_decode(bbP, dataP);\
        };
#else
    #define TYPEDEF_UNSIGNED_INT1(DATA_TYPE_NAME)            typedef MIH_C_UNSIGNED_INT1_T   DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);

    #define TYPEDEF_UNSIGNED_INT2(DATA_TYPE_NAME)            typedef MIH_C_UNSIGNED_INT2_T   DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);

    #define TYPEDEF_UNSIGNED_INT4(DATA_TYPE_NAME)            typedef MIH_C_UNSIGNED_INT4_T   DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);

    #define TYPEDEF_UNSIGNED_INT8(DATA_TYPE_NAME)            typedef MIH_C_UNSIGNED_INT8_T   DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* buffP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);
#endif

/*! \def UNSIGNED_INT1(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_UNSIGNED_INT1_T.
 */
#define UNSIGNED_INT1(VAR_NAME)                             MIH_C_UNSIGNED_INT1_T VAR_NAME;
/*! \def UNSIGNED_INT2(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_UNSIGNED_INT2_T.
 */
#define UNSIGNED_INT2(VAR_NAME)                             MIH_C_UNSIGNED_INT2_T VAR_NAME;
/*! \def UNSIGNED_INT4(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_UNSIGNED_INT4_T.
 */
#define UNSIGNED_INT4(VAR_NAME)                             MIH_C_UNSIGNED_INT4_T VAR_NAME;
/*! \def UNSIGNED_INT8(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_UNSIGNED_INT8_T.
 */
#define UNSIGNED_INT8(VAR_NAME)                             MIH_C_UNSIGNED_INT8_T VAR_NAME;

//----------------------- LIST(DATATYPE) --------------------------------------
void      MIH_C_LIST_LENGTH_encode(Bit_Buffer_t *bbP, u_int16_t lengthP);
void      MIH_C_LIST_LENGTH_decode(Bit_Buffer_t *bbP, u_int16_t *lengthP);
u_int16_t MIH_C_LIST_LENGTH_get_encode_length(u_int16_t lengthP);

#ifdef MIH_C_F1_BASIC_DATA_TYPES_CODEC_C
    /*! \def TYPEDEF_LIST(DATA_TYPE_NAME, MAX_LENGTH)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a LIST of MAX_LENGTH elements of DATA_TYPE_NAME type, and its functions for de/serializing this type.
     */
#define TYPEDEF_LIST(DATA_TYPE_NAME, MAX_LENGTH)    typedef  struct DATA_TYPE_NAME ## _LIST { u_int16_t length; DATA_TYPE_NAME ## _T val[MAX_LENGTH];} DATA_TYPE_NAME ## _LIST_T;\
    extern void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *val);\
    extern void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *val);\
    extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *val, char* bufP);\
    unsigned int DATA_TYPE_NAME ## _LIST2String(DATA_TYPE_NAME ## _LIST_T *listP, char* bufP) {\
        int index = 0;\
        unsigned int buffer_index = 0;\
        while ((index <  listP->length) && (index < MAX_LENGTH)){\
            buffer_index += sprintf(&bufP[buffer_index], "%s (%d) = ", STR(DATA_TYPE_NAME) , index);\
            buffer_index += DATA_TYPE_NAME ## 2String(&listP->val[index], &bufP[buffer_index]);\
            buffer_index += sprintf(&bufP[buffer_index], " ");\
            index = index + 1;\
        }\
        return buffer_index;\
    };\
    inline void DATA_TYPE_NAME ## _LIST_decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _LIST_T *listP) {\
        int index = 0;\
        MIH_C_LIST_LENGTH_decode(bbP, &listP->length);\
        if (listP->length > MAX_LENGTH) {printf("[MIH_C] ERROR DATA_TYPE_NAME ## _LIST_decode num elements in list out of bounds: %d, max is %d\n", listP->length, MAX_LENGTH);\
        } else {\
            while ((index <  listP->length) && (index < MAX_LENGTH)){\
                DATA_TYPE_NAME ## _decode(bbP, &listP->val[index++]);\
            }\
        }\
    };\
    inline void DATA_TYPE_NAME ## _LIST_encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _LIST_T *listP) {\
        int index = 0;\
        if (listP->length > MAX_LENGTH) {\
            printf("[MIH_C] ERROR DATA_TYPE_NAME ## _LIST_encode num elements in list out of bounds: %d, max is %d\n", listP->length, MAX_LENGTH);\
        } else {\
            MIH_C_LIST_LENGTH_encode(bbP, listP->length);\
            while (index <  listP->length) {\
                DATA_TYPE_NAME ## _encode(bbP, &listP->val[index++]);\
            }\
        }\
    };\
    inline void DATA_TYPE_NAME ## _LIST_init(DATA_TYPE_NAME ## _LIST_T *listP) {\
        listP->length = 0;\
        memset(listP->val, 0, MAX_LENGTH*sizeof(DATA_TYPE_NAME ## _T));\
    };

#else
#define TYPEDEF_LIST(DATA_TYPE_NAME, MAX_LENGTH)    typedef  struct DATA_TYPE_NAME ## _LIST { u_int16_t length; DATA_TYPE_NAME ## _T val[MAX_LENGTH];} DATA_TYPE_NAME ## _LIST_T;\
    extern unsigned int DATA_TYPE_NAME ## _LIST2String(DATA_TYPE_NAME ## _LIST_T *listP, char* bufP);\
    extern void         DATA_TYPE_NAME ## _LIST_decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _LIST_T *listP);\
    extern void         DATA_TYPE_NAME ## _LIST_encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _LIST_T *listP);\
    extern void         DATA_TYPE_NAME ## _LIST_init(DATA_TYPE_NAME ## _LIST_T *listP);
#endif
#define LIST(DATA_TYPE_NAME, VAR_NAME)              DATA_TYPE_NAME ## _LIST_T VAR_NAME ## _list;



//----------------------- NULL ------------------------------------------------
#define MIH_C_NULL_T     u_int8_t

//----------------------- OCTET(size) -----------------------------------------
#ifdef MIH_C_F1_BASIC_DATA_TYPES_CODEC_C
    /*! \def TYPEDEF_OCTET(DATA_TYPE_NAME, MAX_LENGTH)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a LIST of MAX_LENGTH octet elements, and its functions for de/serializing this type.
     */
#define TYPEDEF_OCTET(DATA_TYPE_NAME, LENGTH)       typedef struct DATA_TYPE_NAME {u_int8_t val[LENGTH];} DATA_TYPE_NAME  ## _T;\
    unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* bufP) {\
        int index = 0;\
        unsigned int buffer_index = 0;\
        while ( index <  LENGTH ){\
            buffer_index += sprintf(&bufP[buffer_index], "%02X", dataP->val[index]);\
            index = index + 1;\
        }\
        return buffer_index;\
    };\
    inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) { BitBuffer_readMem(bbP, &dataP->val[0], LENGTH);};\
    inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) { BitBuffer_writeMem(bbP, &dataP->val[0], LENGTH);};
#else
#define TYPEDEF_OCTET(DATA_TYPE_NAME, LENGTH)       typedef struct DATA_TYPE_NAME {u_int8_t val[LENGTH];} DATA_TYPE_NAME  ## _T;\
    extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* bufP);\
    extern void         DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
    extern void         DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);
#endif

//----------------------- SEQUENCE(DATATYPE1, DATATYPE2[,...]) : NO MACRO -----

//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.2 General data types
//-----------------------------------------------------------------------------

//----------------------- ENUMERATED ------------------------------------------
TYPEDEF_UNSIGNED_INT1(MIH_C_ENUMERATED)

#ifdef MIH_C_F2_GENERAL_DATA_TYPES_CODEC_C
    /*! \def TYPEDEF_ENUMERATED(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as MIH_C_ENUMERATED_T, and its functions for de/serializing this type.
     */
    #define TYPEDEF_ENUMERATED(DATA_TYPE_NAME)          typedef MIH_C_ENUMERATED_T  DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_ENUMERATED_encode(Bit_Buffer_t* bbP, MIH_C_ENUMERATED_T* dataP);\
        extern inline void MIH_C_ENUMERATED_decode(Bit_Buffer_t* bbP, MIH_C_ENUMERATED_T* dataP);\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_ENUMERATED_encode(bbP, dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {MIH_C_ENUMERATED_decode(bbP, dataP);};
#else
    #define TYPEDEF_ENUMERATED(DATA_TYPE_NAME)          typedef MIH_C_ENUMERATED_T  DATA_TYPE_NAME  ## _T;\
        extern inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);
#endif

/*! \def ENUMERATED(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_ENUMERATED_T.
 */
#define ENUMERATED(VAR_NAME)                                MIH_C_ENUMERATED_T  VAR_NAME;


//----------------------- BOOLEAN ---------------------------------------------
TYPEDEF_ENUMERATED(MIH_C_BOOLEAN)

#define MIH_C_BOOLEAN_TRUE       (MIH_C_ENUMERATED_T)1
#define MIH_C_BOOLEAN_FALSE      (MIH_C_ENUMERATED_T)0
#ifdef MIH_C_F2_GENERAL_DATA_TYPES_CODEC_C
    /*! \def TYPEDEF_BOOLEAN(DATA_TYPE_NAME)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as MIH_C_BOOLEAN_T, and its functions for de/serializing this type.
     */
    #define TYPEDEF_BOOLEAN(DATA_TYPE_NAME)          typedef MIH_C_BOOLEAN_T  DATA_TYPE_NAME  ## _T;\
        extern inline void MIH_C_BOOLEAN_encode(Bit_Buffer_t* bbP, MIH_C_BOOLEAN_T* dataP);\
        extern inline void MIH_C_BOOLEAN_decode(Bit_Buffer_t* bbP, MIH_C_BOOLEAN_T* dataP);\
        unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* bufP) {\
            unsigned int buffer_index = 0;\
            if (*dataP != MIH_C_BOOLEAN_FALSE) {\
                buffer_index += sprintf(&bufP[buffer_index], "TRUE");\
            } else {\
                buffer_index += sprintf(&bufP[buffer_index], "FALSE");\
            }\
            return buffer_index;\
        };\
        inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
            MIH_C_BOOLEAN_encode(bbP, dataP);};\
        inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {MIH_C_ENUMERATED_decode(bbP, dataP);};
#else
    #define TYPEDEF_BOOLEAN(DATA_TYPE_NAME)          typedef MIH_C_BOOLEAN_T  DATA_TYPE_NAME  ## _T;\
        extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* bufP);\
        extern inline void  DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
        extern inline void  DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);
#endif

/*! \def BOOLEAN(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief A variable declaration of type MIH_C_BOOLEAN_T.
 */
#define BOOLEAN(VAR_NAME)                     MIH_C_BOOLEAN_T  VAR_NAME;


//----------------------- OCTET_STRING ----------------------------------------
#ifdef MIH_C_F2_GENERAL_DATA_TYPES_CODEC_C
    /*! \def TYPEDEF_OCTET_STRING(DATA_TYPE_NAME, MAX_LENGTH)
     * \ingroup MIH_C_F1_BASIC_DATA_TYPES
     * \brief Defines DATA_TYPE_NAME as a LIST of MAX_LENGTH char elements, and its functions for de/serializing this type.
     */
#define TYPEDEF_OCTET_STRING(DATA_TYPE_NAME, MAX_LENGTH)    typedef  struct DATA_TYPE_NAME { u_int16_t length; u_int8_t val[MAX_LENGTH];} DATA_TYPE_NAME ## _T;\
    unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *listP, char* bufP) {\
        memcpy(bufP, listP->val, listP->length);\
        bufP[listP->length] = 0;\
        return listP->length;\
    };\
    inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *listP) {\
        MIH_C_LIST_LENGTH_decode(bbP, &listP->length);\
        if (listP->length > MAX_LENGTH) {printf("[MIH_C] ERROR DATA_TYPE_NAME ## _decode String length out of bounds: %d, max is %d\n", listP->length, MAX_LENGTH);\
        } else {\
            BitBuffer_readMem(bbP, listP->val, listP->length);\
        }\
    };\
    inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *listP) {\
        if (listP->length > MAX_LENGTH) {\
            printf("[MIH_C] ERROR DATA_TYPE_NAME ## _encode String length out of bounds: %d, max is %d\n", listP->length, MAX_LENGTH);\
        } else {\
            MIH_C_LIST_LENGTH_encode(bbP, listP->length);\
            BitBuffer_writeMem(bbP, listP->val, listP->length);\
        }\
    };\
    inline void DATA_TYPE_NAME ## _set(DATA_TYPE_NAME ## _T *octet_strP, u_int8_t* strP, u_int16_t lengthP) {\
        if (lengthP > MAX_LENGTH) {\
            printf("[MIH_C] ERROR DATA_TYPE_NAME ## _set String length out of bounds\n");\
            octet_strP->length = 0;\
        } else {\
            octet_strP->length = lengthP;\
            if (strP != NULL) {\
                memcpy((char *)octet_strP->val, (char *)strP, lengthP);\
            } else {\
                memset((char *)octet_strP->val, 0, lengthP);\
            }\
        }\
    };
#else
#define TYPEDEF_OCTET_STRING(DATA_TYPE_NAME, MAX_LENGTH)    typedef  struct DATA_TYPE_NAME { u_int16_t length; u_int8_t val[MAX_LENGTH];} DATA_TYPE_NAME ## _T;\
    extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *listP, char* bufP);\
    extern void         DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *listP);\
    extern void         DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *listP);\
    extern void         DATA_TYPE_NAME ## _set(DATA_TYPE_NAME ## _T *octet_strP, u_int8_t* strP, u_int16_t lengthP);
#endif


//----------------------- PERCENTAGE ------------------------------------------
TYPEDEF_UNSIGNED_INT1(MIH_C_PERCENTAGE);
/*! \var PERCENTAGE(VAR_NAME)
 * \ingroup MIH_C_F1_BASIC_DATA_TYPES
 * \brief Defines VAR_NAME as MIH_C_PERCENTAGE_T.
 */
#define PERCENTAGE(VAR_NAME)                 MIH_C_PERCENTAGE_T  VAR_NAME;

//----------------------- STATUS ----------------------------------------------
TYPEDEF_ENUMERATED(MIH_C_STATUS);
#define MIH_C_STATUS_SUCCESS                 (MIH_C_STATUS_T)0
#define MIH_C_STATUS_UNSPECIFIED_FAILURE     (MIH_C_STATUS_T)1
#define MIH_C_STATUS_REJECTED                (MIH_C_STATUS_T)2
#define MIH_C_STATUS_AUTHORIZATION_FAILURE   (MIH_C_STATUS_T)3
#define MIH_C_STATUS_NETWORK_ERROR           (MIH_C_STATUS_T)4

//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.3 Data types for addresses
//-----------------------------------------------------------------------------
/*! \var MIH_C_UNSIGNED_INT4_T MIH_C_CELL_ID_T
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief This data type identifies a cell uniquely within 3GPP UTRAN and
* consists of radio network controller (RNC)-ID and C-ID as defined in
* 3GPP TS 25.401.
* Valid Range: 0..268435455.
*/
TYPEDEF_UNSIGNED_INT4(MIH_C_CELL_ID)
//-------------------------------------------
/*!
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief The BSS and cell within the BSS are identified by Cell Identity (CI).
* See 3GPP TS 23.003.
*/
TYPEDEF_OCTET(MIH_C_CI, 2)
//-------------------------------------------
/*!
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief The public land mobile network (PLMN) unique identifier.
*        PLMN_ID consists of Mobile Country Code (MCC) and Mobile Network Code (MNC).
*        This is to represent the access network identifier.
*        Coding of PLMN_ID is defined in 3GPP TS 25.413.
*/
TYPEDEF_OCTET(MIH_C_PLMN_ID, 3)
//-------------------------------------------
/*!
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief Location Area Code (LAC) is a fixed length code (of 2 octets)
* identifying a location area within a public landmobile network (PLMN).
* See 3GPP TS 23.003.
*/
TYPEDEF_OCTET(MIH_C_LAC, 2)
//-------------------------------------------
/*! \struct MIH_C_3GPP_2G_CELL_ID
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief A data type to represent a 3GPP 2G cell identifier.
*/
typedef struct MIH_C_3GPP_2G_CELL_ID {
    MIH_C_PLMN_ID_T        plmn_id;
    MIH_C_LAC_T            lac;
    MIH_C_CI_T             ci;
} MIH_C_3GPP_2G_CELL_ID_T;

//-------------------------------------------
/*! \struct  MIH_C_3GPP_3G_CELL_ID
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief A data type to represent a 3GPP 3G cell identifier.
*/
typedef struct MIH_C_3GPP_3G_CELL_ID {
    MIH_C_PLMN_ID_T        plmn_id;
    MIH_C_CELL_ID_T        cell_id;
} MIH_C_3GPP_3G_CELL_ID_T;
//-------------------------------------------
/*! \struct  MIH_C_3GPP_ADDR
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief A data type to represent a 3GPP transport address.
*/
TYPEDEF_OCTET_STRING(MIH_C_3GPP_ADDR, MIH_C_3GPP_ADDR_LENGTH)
//-------------------------------------------
/*! \struct  MIH_C_3GPP2_ADDR
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief A data type to represent a 3GPP2 transport address.
*/
TYPEDEF_OCTET_STRING(MIH_C_3GPP2_ADDR, MIH_C_3GPP2_ADDR_LENGTH)
//-------------------------------------------
/*! \var MIH_C_IANA_ADDR_T
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief Is the address family type of a transport address.
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_IANA_ADDR)
//-------------------------------------------
/*! \struct MIH_C_TRANSPORT_ADDR_T
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief A type to represent a transport address. The
* UNSIGNED_INT(2) is the address type defined in
* http://www.iana.org/assignments/address-family-numbers.
*/
TYPEDEF_OCTET_STRING(MIH_C_TRANSPORT_ADDR_VALUE, 128)
//-------------------------------------------
#ifdef MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC_C
#define TYPEDEF_TRANSPORT_ADDR(DATA_TYPE_NAME)  typedef struct DATA_TYPE_NAME {\
                                                                MIH_C_IANA_ADDR_T              address_family;\
                                                                MIH_C_TRANSPORT_ADDR_VALUE_T   address;\
                                                            } DATA_TYPE_NAME ## _T;\
    unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* bufP) {\
        unsigned int buffer_index = 0;\
        buffer_index += sprintf(&bufP[buffer_index], "IANA = ");\
        buffer_index += MIH_C_IANA_ADDR2String(&dataP->address_family, &bufP[buffer_index]);\
        buffer_index += sprintf(&bufP[buffer_index], "TRANSPORT_ADDR = ");\
        buffer_index += MIH_C_TRANSPORT_ADDR_VALUE2String(&dataP->address, &bufP[buffer_index]);\
        return buffer_index;\
    };\
    inline void DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
        MIH_C_IANA_ADDR_decode(bbP, &dataP->address_family);\
        MIH_C_TRANSPORT_ADDR_VALUE_decode(bbP, &dataP->address);\
    };\
    inline void DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP) {\
        MIH_C_IANA_ADDR_encode(bbP, &dataP->address_family);\
        MIH_C_TRANSPORT_ADDR_VALUE_encode(bbP, &dataP->address);\
    };
#else
#define TYPEDEF_TRANSPORT_ADDR(DATA_TYPE_NAME)  typedef struct DATA_TYPE_NAME {\
                                                                MIH_C_IANA_ADDR_T              address_family;\
                                                                MIH_C_TRANSPORT_ADDR_VALUE_T   address;\
                                                            } DATA_TYPE_NAME ## _T;\
    extern unsigned int DATA_TYPE_NAME ## 2String(DATA_TYPE_NAME ## _T *dataP, char* bufP);\
    extern void         DATA_TYPE_NAME ## _decode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);\
    extern void         DATA_TYPE_NAME ## _encode(Bit_Buffer_t *bbP, DATA_TYPE_NAME ## _T *dataP);
#endif
//-------------------------------------------
/*! \struct  MIH_C_IP_ADDR_T
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief Represents an IP address. The Address Type is either 1 (IPv4) or 2 (IPv6).
*/
//typedef MIH_C_TRANSPORT_ADDR_T   MIH_C_IP_ADDR_T;
TYPEDEF_TRANSPORT_ADDR(MIH_C_IP_ADDR)
//-------------------------------------------
/*! \struct  MIH_C_MAC_ADDR_T
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief Represents a MAC address. The Address Type contains the one used for a specific link layer.
*/
//typedef MIH_C_TRANSPORT_ADDR_T   MIH_C_MAC_ADDR_T;
TYPEDEF_TRANSPORT_ADDR(MIH_C_MAC_ADDR)
//-------------------------------------------
/*! \struct  MIH_C_OTHER_L2_ADDR_T
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief A data type to represent a link-layer address other than the address already defined.
* For example, SSID.
*/
TYPEDEF_OCTET_STRING(MIH_C_OTHER_L2_ADDR, MIH_C_OTHER_L2_ADDR_LENGTH)
//-------------------------------------------

typedef enum  {
                  MIH_C_CHOICE_MAC_ADDR = 0,
                  MIH_C_CHOICE_3GPP_3G_CELL_ID,
                  MIH_C_CHOICE_3GPP_2G_CELL_ID,
                  MIH_C_CHOICE_3GPP_ADDR,
                  MIH_C_CHOICE_3GPP2_ADDR,
                  MIH_C_CHOICE_OTHER_L2_ADDR
              } MIH_C_LINK_ADDR_CHOICE_ENUM_T;
//-------------------------------------------
/*! \struct MIH_C_LINK_ADDR
* \ingroup MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC
* \brief A data type to represent an address of any link layer.
*/
typedef struct MIH_C_LINK_ADDR {
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_MAC_ADDR_T         mac_addr;
        MIH_C_3GPP_3G_CELL_ID_T  _3gpp_3g_cell_id;
        MIH_C_3GPP_2G_CELL_ID_T  _3gpp_2g_cell_id;
        MIH_C_3GPP_ADDR_T        _3gpp_addr;
        MIH_C_3GPP2_ADDR_T       _3gpp2_addr;
        MIH_C_OTHER_L2_ADDR_T    other_l2_addr;
    } _union;
} MIH_C_LINK_ADDR_T;




//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.4 Data types for links (MAINLY because of precedence definition)
//-----------------------------------------------------------------------------
/*!
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents percentage of battery charge remaining.

*/
TYPEDEF_INTEGER1(MIH_C_BATT_LEVEL)
//-------------------------------------------
/*!
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Channel identifier as defined in the specific link technology (e.g., standards
*        development organization (SDO)).
*        Valid Range: 0..65535
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_CHANNEL_ID)
//-------------------------------------------
#define MIH_C_CONFIG_STATUS_SUCCESS MIH_C_BOOLEAN_TRUE
#define MIH_C_CONFIG_STATUS_ERROR   MIH_C_BOOLEAN_FALSE
/*! \var  MIH_C_CONFIG_STATUS_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief The status of link parameter configuration.
*        TRUE: Success
*        FALSE: Error
*/
TYPEDEF_BOOLEAN(MIH_C_CONFIG_STATUS)
//-------------------------------------------
/*! \var  MIH_C_DEVICE_INFO_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A non-NULL terminated string whose length shall not exceed 253 octets, representing
*        information on manufacturer, model number, revision number of the software/firmware
*        and serial number in displayable text.
*/
TYPEDEF_OCTET_STRING(MIH_C_DEVICE_INFO, 253)
//-------------------------------------------
#define MIH_C_BIT_DEVICE_INFO   MIH_C_BIT_0_VALUE
#define MIH_C_BIT_BATT_LEVEL    MIH_C_BIT_1_VALUE
/*!
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A list of device status request.
*        Bitmap Values:
*        Bit 0: DEVICE_INFO
*        Bit 1: BATT_LEVEL
*        Bit 2–15: (Reserved)
*/
TYPEDEF_BITMAP16(MIH_C_DEV_STATES_REQ)
//-------------------------------------------
/*! \struct  MIH_C_DEV_STATE_RSP_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents a device status.
*/
typedef struct MIH_C_DEV_STATE_RSP {
    MIH_C_CHOICE_T          choice;
    union  {
        MIH_C_DEVICE_INFO_T      device_info;
        MIH_C_BATT_LEVEL_T       batt_level;
    } _union;
} MIH_C_DEV_STATE_RSP_T;
//-------------------------------------------
/*!
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Time (in ms) to elapse before an action needs to be taken. A value of 0 indicates
*        that the action will be taken immediately. Time elapsed will be calculated from the
*        instance the command arrives until the time when the execution of the action is carried out.
*        Valid Range: 0..65535
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_LINK_AC_EX_TIME)
//-------------------------------------------
/*!
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Link action result.
*        0: Success
*        1: Failure
*        2: Refused
*        3: Incapable
*/
TYPEDEF_ENUMERATED(MIH_C_LINK_AC_RESULT)
#define MIH_C_LINK_AC_RESULT_SUCCESS   (MIH_C_LINK_AC_RESULT_T)0
#define MIH_C_LINK_AC_RESULT_FAILURE   (MIH_C_LINK_AC_RESULT_T)1
#define MIH_C_LINK_AC_RESULT_REFUSED   (MIH_C_LINK_AC_RESULT_T)2
#define MIH_C_LINK_AC_RESULT_INCAPABLE (MIH_C_LINK_AC_RESULT_T)3
//-------------------------------------------
#define MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN          MIH_C_BIT_0_VALUE
#define MIH_C_BIT_LINK_AC_ATTR_LINK_RES_RETAIN    MIH_C_BIT_1_VALUE
#define MIH_C_BIT_LINK_AC_ATTR_DATA_FWD_REQ       MIH_C_BIT_2_VALUE
/*!
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Link action attribute that can be executed along with a valid link action.
*        Detail description of each attribute is in Table F.6.
*        Bitmap Values:
*        Bit 0: LINK_SCAN
*        Bit 1: LINK_RES_RETAIN
*        Bit 2: DATA_FWD_REQ
*        Bit 3–7: (Reserved)
*/
TYPEDEF_BITMAP8(MIH_C_LINK_AC_ATTR)
//-------------------------------------------
/*!
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief An action for a link. The meaning of each link action is defined in Table F.5.
*        0: NONE
*        1: LINK_DISCONNECT
*        2: LINK_LOW_POWER
*        3: LINK_POWER_DOWN
*        4: LINK_POWER_UP
*        5–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_AC_TYPE)
#define MIH_C_LINK_AC_TYPE_NONE                      (MIH_C_LINK_AC_TYPE_T)0
#define MIH_C_LINK_AC_TYPE_LINK_DISCONNECT           (MIH_C_LINK_AC_TYPE_T)1
#define MIH_C_LINK_AC_TYPE_LINK_LOW_POWER            (MIH_C_LINK_AC_TYPE_T)2
#define MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN           (MIH_C_LINK_AC_TYPE_T)3
#define MIH_C_LINK_AC_TYPE_LINK_POWER_UP             (MIH_C_LINK_AC_TYPE_T)4
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
#define MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR            (MIH_C_LINK_AC_TYPE_T)5
#define MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES   (MIH_C_LINK_AC_TYPE_T)6
#define MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES (MIH_C_LINK_AC_TYPE_T)7
#endif
//-------------------------------------------
/*!
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents the link type.a
*        Number assignments:
*        0: Reserved
*        1: Wireless - GSM
*        2: Wireless - GPRS
*        3: Wireless - EDGE
*        15: Ethernet
*        18: Wireless - Other
*        19: Wireless - IEEE 802.11
*        22: Wireless - CDMA2000
*        23: Wireless - UMTS
*        24: Wireless - cdma2000-HRPD
*        25: Wireless - LTE (MEDIEVAL)
*        27: Wireless - IEEE 802.16
*        28: Wireless - IEEE 802.20
*        29: Wireless - IEEE 802.22
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_TYPE)
#define MIH_C_WIRELESS_GSM            (MIH_C_LINK_TYPE_T)1
#define MIH_C_WIRELESS_GPRS           (MIH_C_LINK_TYPE_T)2
#define MIH_C_WIRELESS_EDGE           (MIH_C_LINK_TYPE_T)3
#define MIH_C_ETHERNET                (MIH_C_LINK_TYPE_T)15
#define MIH_C_WIRELESS_OTHER          (MIH_C_LINK_TYPE_T)18
#define MIH_C_WIRELESS_IEEE802_11     (MIH_C_LINK_TYPE_T)19
#define MIH_C_WIRELESS_CDMA_2000      (MIH_C_LINK_TYPE_T)22
//#define MIH_C_WIRELESS_UMTS           (MIH_C_LINK_TYPE_T)23
// Temp - MW modified to test LTE for MEDIEVAL
#define MIH_C_WIRELESS_UMTS           (MIH_C_LINK_TYPE_T)25
#define MIH_C_WIRELESS_CDMA_2000_HRPD (MIH_C_LINK_TYPE_T)24
#define MIH_C_WIRELESS_LTE            (MIH_C_LINK_TYPE_T)25
#define MIH_C_WIRELESS_IEEE802_16     (MIH_C_LINK_TYPE_T)27
#define MIH_C_WIRELESS_IEEE802_20     (MIH_C_LINK_TYPE_T)28
#define MIH_C_WIRELESS_IEEE802_22     (MIH_C_LINK_TYPE_T)29
//-------------------------------------------
/*! \struct  MIH_C_LINK_ID_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief The identifier of a link that is not associated with the peer node.
*        The LINK_ADDR contains the address of this link.
*/
typedef struct MIH_C_LINK_ID {
    MIH_C_LINK_TYPE_T           link_type;
    MIH_C_LINK_ADDR_T           link_addr;
} MIH_C_LINK_ID_T;
//-------------------------------------------
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
/*! \var MIH_C_PORT_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief 2 octets defining the port used by the transport protocol
*/
TYPEDEF_OCTET(MIH_C_PORT, 2)
//-------------------------------------------
/*! \struct MIH_C_IP_TUPLE_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief Tuple consisting on an IP address and the port
*/
typedef struct MIH_C_IP_TUPLE {
    MIH_C_IP_ADDR_T              ip_addr;
    MIH_C_PORT_T                 port;
} MIH_C_IP_TUPLE_T;
//-------------------------------------------
/*! \var MIH_C_PROTO_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief The transport protocol used:
*    0: TCP
*    1: UDP
*/
TYPEDEF_ENUMERATED(MIH_C_PROTO)
#define MIH_C_PROTO_TCP   (MIH_C_PROTO_T)0
#define MIH_C_PROTO_UDP   (MIH_C_PROTO_T)1
//-------------------------------------------
/*! \struct MIH_C_FLOW_ID_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief Five tuple, consisting on the source and destination address
* and ports plus the transport protocol used.
*/
/*typedef struct MIH_C_FLOW_ID {
    MIH_C_IP_TUPLE_T              source_addr;
    MIH_C_IP_TUPLE_T              dest_addr;
    MIH_C_PROTO_T                 transport_protocol;
} MIH_C_FLOW_ID_T;*/
TYPEDEF_UNSIGNED_INT4(MIH_C_FLOW_ID)
//-------------------------------------------
/*! \struct MIH_C_MARK_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief 6 Bits (IPv4) or 20 Bits (IPv6) mask to be applied to the DSCP
* or Flow Label field of IPv4/v6 header.
*/
typedef struct MIH_C_MARK {
    MIH_C_CHOICE_T          choice;
    union  {
        BITMAP6(dscp_mask);
        BITMAP20(flow_label_mask);
    } _union;
} MIH_C_MARK_T;
//-------------------------------------------
/*! \var MIH_C_MAX_DELAY_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief Maximum delay supported by the flow in ms.
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_MAX_DELAY)
//-------------------------------------------
/*! \var MIH_C_BITRATE_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief A type to represent the maximum data rate in kb/s.
*  Valid Range: 0 –– 2^32 – 1

*/
TYPEDEF_UNSIGNED_INT4(MIH_C_BITRATE)
//-------------------------------------------
/*! \var MIH_C_JITTER_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief A type to represent the packet transfer delay jitter in ms.
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_JITTER)
//-------------------------------------------
/*! \var MIH_C_PKT_LOSS_RATE_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief A type to represent the packet loss rate. The loss rate is equal
* to the integer part of the result of multiplying --100 times the
* log10 of the ratio between the number of packets lost and the
* total number of packets transmitted.
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_PKT_LOSS_RATE)
//-------------------------------------------
/*! \var MIH_C_COS_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief COS Class to be used for queuing.
*  To be filled for LTE and WLAN
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_COS)
//-------------------------------------------
/*! \var MIH_C_DROP_ELIGIBILITY_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief 0 means the frames are not eligible to discarding.
* 1 means frames are eligible for discarding.
*/
TYPEDEF_BOOLEAN(MIH_C_DROP_ELIGIBILITY)
//-------------------------------------------
/*! \var MIH_C_MULTICAST_ENABLE_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief Identifies if a flow is multicast.
*  0: is not multicast
*  1: is multicast
*/
TYPEDEF_BOOLEAN(MIH_C_MULTICAST_ENABLE)
//-------------------------------------------
/*! \var MIH_C_JUMBO_ENABLE_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief Identifies if JUMBO must be activated
*  0: Deactivate Jumbo
*  1: Activate Jumbo
*/
TYPEDEF_BOOLEAN(MIH_C_JUMBO_ENABLE)
#endif
//-------------------------------------------
/*! \var MIH_C_LINK_DATA_RATE_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent the maximum data rate in kb/s.
*        Valid Range: 0 – 232–1
*/
TYPEDEF_UNSIGNED_INT4(MIH_C_LINK_DATA_RATE)
//-------------------------------------------
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
/*! \struct MIH_C_QOS_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief The choice of delay and bitrate corresponds to a new flow being
*  signalled through the MIH_SAP, the COS parameter is used after being processed by the WP3 AQM.
*/
typedef struct MIH_C_QOS {
    MIH_C_CHOICE_T          choice;
    union  {
        struct {
            MIH_C_MAX_DELAY_T      max_delay;
            MIH_C_BITRATE_T        bitrate;
            MIH_C_JITTER_T         jitter;
            MIH_C_PKT_LOSS_RATE_T  pkt_loss;
        }                          mark_qos;
        MIH_C_COS_T                cos;
    } _union;
} MIH_C_QOS_T;
//-------------------------------------------
/*! \struct MIH_C_RESOURCE_DESC_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief The choice of RESOURCE_DESC is used when the LINK_ACTIVATE_RESOURCES
* or DEACTIVATE action is selected in order to identify and
* configure the flow subject to action
*/
typedef struct MIH_C_RESOURCE_DESC {
    MIH_C_LINK_ID_T         link_id;
    MIH_C_FLOW_ID_T         flow_id;
    MIH_C_CHOICE_T          choice_link_data_rate;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_LINK_DATA_RATE_T   link_data_rate;
    } _union_link_data_rate;
    MIH_C_CHOICE_T          choice_qos;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_QOS_T          qos;
    } _union_qos;
    MIH_C_CHOICE_T          choice_jumbo_enable;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_JUMBO_ENABLE_T     jumbo_enable;
    } _union_jumbo_enable;
    MIH_C_CHOICE_T          choice_multicast_enable;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_MULTICAST_ENABLE_T multicast_enable;
    } _union_multicast_enable;
} MIH_C_RESOURCE_DESC_T;
//-------------------------------------------
/*! \struct MIH_C_FLOW_ATTRIBUTE_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief The choice of FLOW_ATTRIBUTE is used when the FLOW_ATTR action is
* selected in order to provide the mark and multicast configuration
* to be set up for the flow.
*/
typedef struct MIH_C_FLOW_ATTRIBUTE {
    MIH_C_FLOW_ID_T         flow_id;
    MIH_C_CHOICE_T          choice_multicast_enable;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_MULTICAST_ENABLE_T multicast_enable;
    } _union_multicast_enable;
    MIH_C_CHOICE_T          choice_mark_qos;
    union  {
        MIH_C_NULL_T             null_attr;
        struct {
            MIH_C_MARK_T         mark;
            MIH_C_QOS_T          qos;
        }                        mark_qos;
    } _union_mark_qos;
    MIH_C_CHOICE_T                      choice_mark_drop_eligibility;
    union  {
        MIH_C_NULL_T                    null_attr;
        struct {
            MIH_C_MARK_T                mark;
            MIH_C_DROP_ELIGIBILITY_T    drop_eligibility;
        }                               mark_drop_eligibility;
    } _union_mark_drop_eligibility;

} MIH_C_FLOW_ATTRIBUTE_T;
//-------------------------------------------
/*! \struct MIH_C_LINK_AC_PARAM_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief The choice of FLOW_ATTRIBUTE is used when the FLOW_ATTR action is selected
*  in order to provide the mark and multicast configuration to be set up for the flow.
*  The choice of RESOURCE_DESC is used when LINK_ACTIVATE_RESOURCES or
*  LINK_DEACTIVATE_RESOURCES actions are used.
*/
typedef struct MIH_C_LINK_AC_PARAM {
    MIH_C_CHOICE_T          choice;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_FLOW_ATTRIBUTE_T   flow_attribute;
        MIH_C_RESOURCE_DESC_T    resource_desc;
    } _union;
} MIH_C_LINK_AC_PARAM_T;
#endif
//-------------------------------------------
/*! \struct  MIH_C_LINK_ACTION_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Link action.
*/
typedef struct MIH_C_LINK_ACTION {
    MIH_C_LINK_AC_TYPE_T           link_ac_type;
    MIH_C_LINK_AC_ATTR_T           link_ac_attr;
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
    MIH_C_LINK_AC_PARAM_T          link_ac_param; /*!<  \brief extension of MEDIEVAL PROJECT  */
#endif
} MIH_C_LINK_ACTION_T;
//-------------------------------------------
/*! \struct MIH_C_LINK_ACTION_REQ_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A set of handover action request parameters. The choice of LINK_ADDR is to
*        provide PoA address information when the LINK_ACTION contains the attribute for  DATA_FWD_REQ.
*/
typedef struct MIH_C_LINK_ACTION_REQ {
    MIH_C_LINK_ID_T              link_id;
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_LINK_ADDR_T        link_addr;
    } _union;

    MIH_C_LINK_ACTION_T          link_action;
    MIH_C_LINK_AC_EX_TIME_T      link_action_ex_time;
} MIH_C_LINK_ACTION_REQ_T;
//-------------------------------------------
/*! \struct  MIH_C_SIG_STRENGTH_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents the signal strength in dBm unit or its relative value in an arbitrary percentage scale.
*/
typedef struct MIH_C_SIG_STRENGTH {
    MIH_C_CHOICE_T               choice;
    union  {
        INTEGER1(dbm)
        PERCENTAGE(percentage)
    } _union;
} MIH_C_SIG_STRENGTH_T;
#define MIH_C_SIG_STRENGTH_CHOICE_DBM        (MIH_C_CHOICE_T)0
#define MIH_C_SIG_STRENGTH_CHOICE_PERCENTAGE (MIH_C_CHOICE_T)1
//-------------------------------------------
/*! \var  MIH_C_NETWORK_ID_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent a network identifier.
*        A non-NULL terminated string whose length shall not exceed 253 octets.
*/
TYPEDEF_OCTET_STRING(MIH_C_NETWORK_ID, 253)
//-------------------------------------------
/*! \struct  MIH_C_LINK_SCAN_RSP_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents a scan response. The LINK_ADDR contains the PoA link address.
*        The PoA belongs to the NETWORK_ID with the given SIG_STRENGTH.
*/
typedef struct MIH_C_LINK_SCAN_RSP {
    MIH_C_LINK_ADDR_T            link_addr;
    MIH_C_NETWORK_ID_T           network_id;
    MIH_C_SIG_STRENGTH_T         sig_strength;
} MIH_C_LINK_SCAN_RSP_T;
//-------------------------------------------
/*! \var MIH_C_LINK_SCAN_RSP_T
* \brief
*/
TYPEDEF_LIST(MIH_C_LINK_SCAN_RSP, MIH_C_LINK_SCAN_RSP_LENGTH)
//-------------------------------------------
/*! \struct  MIH_C_LINK_ACTION_RSP_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A set of link action returned results.
*/
typedef struct MIH_C_LINK_ACTION_RSP {
    MIH_C_LINK_ID_T              link_id;
    MIH_C_LINK_AC_RESULT_T       link_ac_result;
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_NULL_T             null_attr;
        LIST(MIH_C_LINK_SCAN_RSP, link_scan_rsp)
    } _union;
} MIH_C_LINK_ACTION_RSP_T;
//-------------------------------------------
#define MIH_C_BIT_LINK_EVENT_SUBSCRIBE      MIH_C_BIT_1_VALUE
#define MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE    MIH_C_BIT_2_VALUE
#define MIH_C_BIT_LINK_GET_PARAMETERS       MIH_C_BIT_3_VALUE
#define MIH_C_BIT_LINK_CONFIGURE_THRESHOLDS MIH_C_BIT_4_VALUE
#define MIH_C_BIT_LINK_ACTION               MIH_C_BIT_5_VALUE
/*! \var MIH_C_LINK_CMD_LIST_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A list of link commands.
*        Bitmap Values:
*        Bit 0: Reserved
*        Bit 1: Link_Event_Subscribe
*        Bit 2: Link_Event_Unsubscribe
*        Bit 3: Link_Get_Parameters
*        Bit 4: Link_Configure_Thresholds
*        Bit 5: Link_Action
*        Bit 6-31: (Reserved)
*/
TYPEDEF_BITMAP32(MIH_C_LINK_CMD_LIST)
//-------------------------------------------
/*! \var MIH_C_TH_ACTION_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief
*        0: Set normal threshold
*        1: Set one-shot threshold
*        2: Cancel threshold
*/
TYPEDEF_ENUMERATED(MIH_C_TH_ACTION)
#define MIH_C_SET_NORMAL_THRESHOLD      (MIH_C_TH_ACTION_T)0
#define MIH_C_SET_ONE_SHOT_THRESHOLD    (MIH_C_TH_ACTION_T)1
#define MIH_C_CANCEL_THRESHOLD          (MIH_C_TH_ACTION_T)2
//-------------------------------------------
/*! \var MIH_C_THRESHOLD_VAL_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Threshold value. The format of the media-dependent value is defined in the
*        respective media specification standard and the equivalent number of bits (i.e.,
*        first bits) of this data type is used. In case that there are remaining unused bits
*        in the data type, these are marked as all-zeros (‘0’).
*        Valid Range: 0..65535
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_THRESHOLD_VAL)

//-------------------------------------------
/*! \var MIH_C_THRESHOLD_XDIR_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief The direction the threshold is to be crossed.
*        0: ABOVE_THRESHOLD
*        1: BELOW_THRESHOLD
*        2–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_THRESHOLD_XDIR)
#define MIH_C_ABOVE_THRESHOLD           (MIH_C_THRESHOLD_XDIR_T)0
#define MIH_C_BELOW_THRESHOLD           (MIH_C_THRESHOLD_XDIR_T)1
//-------------------------------------------
/*! \struct  MIH_C_THRESHOLD_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A link threshold. The threshold is considered crossed when the value of the
*        link parameter passes the threshold in the specified direction.
*/
typedef struct MIH_C_THRESHOLD {
    MIH_C_THRESHOLD_VAL_T        threshold_val;
    MIH_C_THRESHOLD_XDIR_T       threshold_xdir;
} MIH_C_THRESHOLD_T;
//-------------------------------------------
/*! \var MIH_C_TIMER_INTERVAL_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief This timer value (ms) is used to set the interval between periodic reports.
*        Valid Range: 0..65535
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_TIMER_INTERVAL)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_GEN_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a generic link parameter that is applicable to any link type.
*        0: Data Rate—the parameter value is represented as a DATA_RATE.
*        1: Signal Strength—the parameter value is represented as a SIG_STRENGTH.
*        2: Signal over interference plus noise ratio (SINR)—the parameter value is
*           represented as an UNSIGNED_INT(2).
*        3:Throughput (the number of bits successfully received divided by the time it
*          took to transmit them over the medium)—the parameter value is represented as
*          an UNSIGNED_INT(2).
*        4: Packet Error Rate (representing the ratio between the number of frames received
*           in error and the total number of frames transmitted in a link population of interest)
*           the parameter value is represented as a PERCENTAGE.
*        5–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_GEN)
#define MIH_C_LINK_PARAM_GEN_DATA_RATE         (MIH_C_LINK_PARAM_GEN_T)0
#define MIH_C_LINK_PARAM_GEN_SIGNAL_STRENGTH   (MIH_C_LINK_PARAM_GEN_T)1
#define MIH_C_LINK_PARAM_GEN_SINR              (MIH_C_LINK_PARAM_GEN_T)2
#define MIH_C_LINK_PARAM_GEN_THROUGHPUT        (MIH_C_LINK_PARAM_GEN_T)3
#define MIH_C_LINK_PARAM_GEN_PACKET_ERROR_RATE (MIH_C_LINK_PARAM_GEN_T)4
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_QOS_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent QOS_LIST parameters.
*        0: Maximum number of differentiable classes of service supported.
*        1: Minimum packet transfer delay for all CoS, the minimum delay over a class
*           population of interest.
*        2: Average packet transfer delay for all CoS, the arithmetic mean of the delay
*           over a class population of interest. (See B.3.4)
*        3: Maximum packet transfer delay for all CoS, the maximum delay over a class
*           population of interest.
*        4: Packet transfer delay jitter for all CoS, the standard deviation of the delay
*           over a class population of interest. (See B.3.5.)
*        5: Packet loss rate for all CoS, the ratio between the number of frames that are
*           transmitted but not received and the total number of frames transmitted over
*           a class population of interest. (See B.3.2.)
*        6–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_QOS)
#define MIH_C_LINK_PARAM_QOS_MAX_NUM_DIF_COS_SUPPORTED            (MIH_C_LINK_PARAM_QOS_T)0
#define MIH_C_LINK_PARAM_QOS_MIN_PACKET_TRANSFER_DELAY_ALL_COS    (MIH_C_LINK_PARAM_QOS_T)1
#define MIH_C_LINK_PARAM_QOS_AVG_PACKET_TRANSFER_DELAY_ALL_COS    (MIH_C_LINK_PARAM_QOS_T)2
#define MIH_C_LINK_PARAM_QOS_MAX_PACKET_TRANSFER_DELAY_ALL_COS    (MIH_C_LINK_PARAM_QOS_T)3
#define MIH_C_LINK_PARAM_QOS_STD_DEVIATION_PACKET_TRANSFER_DELAY  (MIH_C_LINK_PARAM_QOS_T)4
#define MIH_C_LINK_PARAM_QOS_PACKET_LOSS_RATE_ALL_COS_FRAME_RATIO (MIH_C_LINK_PARAM_QOS_T)5
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_GG_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for GSM and GPRS. See 3GPP TS 25.008.
*        0: RxQual
*        1: RsLev
*        2: Mean BEP
*        3: StDev BEP
*        4-255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_GG)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_EDGE_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for EDGE.
*        0-255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_EDGE)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_ETH_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for Ethernet.
*        0-255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_ETH)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_802_11_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for IEEE 802.11.
*        0: RSSI of the beacon channel, as defined in IEEE Std 802.11-2007.
*           (This is applicable only for an MN.)
*        1: No QoS resource available. The corresponding LINK_PARAM_VAL is BOOLEAN set to TRUE when
*           no QoS resources available. (This applicable when the traffic stream to be transmitted
*           is on an access category configured for mandatory admission control and the request for
*           bandwidth was denied by the available APs in the access network).
*        2: Multicast packet loss rate.
*        3–255: (Reserved)
*        Medieval extensions:
*           3: System Load. Percentage of usage load present at the system.
*           4:Number registered users.
*           5:Number active users.
*           6:Congestion window of users. Ordered list of the CW used by the clients, ordered according to
*             highest value of MAC address.
*           7:Transmission rate of users. Ordered list of the transmission rate used by the clients, ordered
*             according to highest value of MAC address.
*           8-255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_802_11)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_C2K_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for CDMA2000.
*        0: PILOT_STRENGTH
*        1–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_C2K)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_FDD_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for UMTS. See 3GPP TS 25.215.
*        0: CPICH RSCP
*        1: PCCPCH RSCP
*        2: UTRA carrier RSSI
*        3: GSM carrier RSSI
*        4: CPICH Ec/No
*        5: Transport channel BLER
*        6: user equipment (UE) transmitted power
*        7–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_FDD)
#define MIH_C_LINK_PARAM_FDD_CPICH_RSCP              (MIH_C_LINK_PARAM_FDD_T)0
#define MIH_C_LINK_PARAM_FDD_PCCPCH_RSCP             (MIH_C_LINK_PARAM_FDD_T)1
#define MIH_C_LINK_PARAM_FDD_UTRA_CARRIER_RSSI       (MIH_C_LINK_PARAM_FDD_T)2
#define MIH_C_LINK_PARAM_FDD_GSM_CARRIER_RSSI        (MIH_C_LINK_PARAM_FDD_T)3
#define MIH_C_LINK_PARAM_FDD_CPICH_EC_NO             (MIH_C_LINK_PARAM_FDD_T)4
#define MIH_C_LINK_PARAM_FDD_TRANSPORT_CHANNEL_BLER  (MIH_C_LINK_PARAM_FDD_T)5
#define MIH_C_LINK_PARAM_FDD_UE_TRANMITTED_POWER     (MIH_C_LINK_PARAM_FDD_T)6
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_HRPD_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for CDMA2000 HRPD.
*        0: PILOT_STRENGTH
*        1–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_HRPD)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_802_16_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for IEEE 802.16.
*        0–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_802_16)
//-------------------------------------------
/*! \var MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for IEEE 802.20.
*        0–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_802_20)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_802_22_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A type to represent a link parameter for IEEE 802.22.
*        0–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_802_22)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_LTE_T
* \ingroup MIH_C_MEDIEVAL_EXTENSIONS
* \brief A type to represent a link parameter for LTE.
*
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_PARAM_LTE)
#define MIH_C_LINK_PARAM_LTE_UE_RSRP                              0
#define MIH_C_LINK_PARAM_LTE_UE_RSRQ                              1
#define MIH_C_LINK_PARAM_LTE_UE_CQI                               2
#define MIH_C_LINK_PARAM_LTE_AVAILABLE_BW                         3
#define MIH_C_LINK_PARAM_LTE_PACKET_DELAY                         4
#define MIH_C_LINK_PARAM_LTE_PACKET_LOSS_RATE                     5
#define MIH_C_LINK_PARAM_LTE_L2_BUFFER_STATUS                     6
#define MIH_C_LINK_PARAM_LTE_MOBILE_NODE_CAPABILITIES             7
#define MIH_C_LINK_PARAM_LTE_EMBMS_CAPABILITY                     8
#define MIH_C_LINK_PARAM_LTE_JUMBO_FEASIBILITY                    9
#define MIH_C_LINK_PARAM_LTE_JUMBO_SETUP_STATUS                   10
#define MIH_C_LINK_PARAM_LTE_NUM_ACTIVE_EMBMS_RECEIVERS_PER_FLOW  11

//-------------------------------------------
/*! \struct  MIH_C_LINK_PARAM_TYPE_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Measurable link parameter for which thresholds are being set.
*/
#define MIH_C_LINK_PARAM_TYPE_CHOICE_GEN    (MIH_C_CHOICE_T)0
#define MIH_C_LINK_PARAM_TYPE_CHOICE_QOS    (MIH_C_CHOICE_T)1
#define MIH_C_LINK_PARAM_TYPE_CHOICE_GG     (MIH_C_CHOICE_T)2
#define MIH_C_LINK_PARAM_TYPE_CHOICE_EDGE   (MIH_C_CHOICE_T)3
#define MIH_C_LINK_PARAM_TYPE_CHOICE_ETH    (MIH_C_CHOICE_T)4
#define MIH_C_LINK_PARAM_TYPE_CHOICE_802_11 (MIH_C_CHOICE_T)5
#define MIH_C_LINK_PARAM_TYPE_CHOICE_C2K    (MIH_C_CHOICE_T)6
#define MIH_C_LINK_PARAM_TYPE_CHOICE_FDD    (MIH_C_CHOICE_T)7
#define MIH_C_LINK_PARAM_TYPE_CHOICE_HRPD   (MIH_C_CHOICE_T)8
#define MIH_C_LINK_PARAM_TYPE_CHOICE_802_16 (MIH_C_CHOICE_T)9
#define MIH_C_LINK_PARAM_TYPE_CHOICE_802_20 (MIH_C_CHOICE_T)10
#define MIH_C_LINK_PARAM_TYPE_CHOICE_802_22 (MIH_C_CHOICE_T)11
#define MIH_C_LINK_PARAM_TYPE_CHOICE_LTE    (MIH_C_CHOICE_T)12

typedef struct MIH_C_LINK_PARAM_TYPE {
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_LINK_PARAM_GEN_T      link_param_gen;
        MIH_C_LINK_PARAM_QOS_T      link_param_qos;
        MIH_C_LINK_PARAM_GG_T       link_param_gg;
        MIH_C_LINK_PARAM_EDGE_T     link_param_edge;
        MIH_C_LINK_PARAM_ETH_T      link_param_eth;
        MIH_C_LINK_PARAM_802_11_T   link_param_802_11;
        MIH_C_LINK_PARAM_C2K_T      link_param_c2k;
        MIH_C_LINK_PARAM_FDD_T      link_param_fdd;
        MIH_C_LINK_PARAM_HRPD_T     link_param_hrpd;
        MIH_C_LINK_PARAM_802_16_T   link_param_802_16;
        MIH_C_LINK_PARAM_802_20_T   link_param_802_20;
        MIH_C_LINK_PARAM_802_22_T   link_param_802_22;
        MIH_C_LINK_PARAM_LTE_T      link_param_lte;
    } _union;
} MIH_C_LINK_PARAM_TYPE_T;
//-------------------------------------------
/*! \var MIH_C_THRESHOLD_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_THRESHOLD, MIH_C_THRESHOLD_LIST_LENGTH)
/*! \struct  MIH_C_LINK_CFG_PARAM_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A link configuration parameter.
*        TH_ACTION indicates what action to apply to the listed thresholds.
*        When “Cancel threshold” is selected and no thresholds are specified, then all
*        currently configured thresholds for the given LINK_PARAM_TYPE are cancelled.
*        When “Cancel threshold” is selected and thresholds are specified only those
*        configured thresholds for the given LINK_PARAM_TYPE and whose threshold value
*        match what was specified are cancelled.
*        With “Set one-shot threshold” the listed thresholds are first set and then each of
*        the threshold is cancelled as soon as it is crossed for the first time.
*/
typedef struct MIH_C_LINK_CFG_PARAM {
    MIH_C_LINK_PARAM_TYPE_T      link_param_type;
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_TIMER_INTERVAL_T   timer_interval;
#ifdef RAL_SAME_AS_MEDIEVAL_PROJECT_BUT_I_THINK_THIS_IS_AN_ERROR
        LIST(MIH_C_LINK_SCAN_RSP, link_scan_rsp)
#endif
    } _union;
    MIH_C_TH_ACTION_T             th_action;
    LIST(MIH_C_THRESHOLD, threshold)
} MIH_C_LINK_CFG_PARAM_T;
//-------------------------------------------
/*! \struct  MIH_C_LINK_CFG_STATUS_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief The status of link parameter configuration for each threshold specified in the THRESHOLD.
*/
typedef struct MIH_C_LINK_CFG_STATUS {
    MIH_C_LINK_PARAM_TYPE_T      link_param_type;
    MIH_C_THRESHOLD_T            threshold;
    MIH_C_CONFIG_STATUS_T        config_status;
} MIH_C_LINK_CFG_STATUS_T;
//-------------------------------------------
#define MIH_C_BIT_NUMBER_OF_CLASSES_OF_SERVICE_SUPPORTED    MIH_C_BIT_0_VALUE
#define MIH_C_BIT_NUMBER_OF_QUEUES_SUPPORTED                MIH_C_BIT_1_VALUE
/*! \var MIH_C_LINK_DESC_REQ_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A set of link descriptors.
*        Bitmap Values:
*        Bit 0: Number of Classes of Service Supported
*        Bit 1: Number of Queues Supported
*        Bits 2–15: (Reserved)
*/
TYPEDEF_BITMAP16(MIH_C_LINK_DESC_REQ)
//-------------------------------------------
/*! \var MIH_C_NUM_COS_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief The maximum number of differentiable classes of service supported.
*        Valid Range: 0..255
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_NUM_COS)
//-------------------------------------------
/*! \var MIH_C_NUM_QUEUE_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief The number of transmit queues supported.
*        Valid Range: 0..255
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_NUM_QUEUE)
//-------------------------------------------
/*! \struct  MIH_C_LINK_DESC_RSP_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Descriptors of a link.
*/
typedef struct MIH_C_LINK_DESC_RSP {
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_NUM_COS_T          num_cos;
        MIH_C_NUM_QUEUE_T        num_queue;
    } _union;
} MIH_C_LINK_DESC_RSP_T;
//-------------------------------------------
/*! \var MIH_C_LINK_DN_REASON_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents the reason of a link down event.
*        See Table F.7 for the enumeration values.
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_DN_REASON)
//-------------------------------------------
#define MIH_C_BIT_LINK_DETECTED            MIH_C_BIT_0_VALUE
#define MIH_C_BIT_LINK_UP                  MIH_C_BIT_1_VALUE
#define MIH_C_BIT_LINK_DOWN                MIH_C_BIT_2_VALUE
#define MIH_C_BIT_LINK_PARAMETERS_REPORT   MIH_C_BIT_3_VALUE
#define MIH_C_BIT_LINK_GOING_DOWN          MIH_C_BIT_4_VALUE
#define MIH_C_BIT_LINK_HANDOVER_IMMINENT   MIH_C_BIT_5_VALUE
#define MIH_C_BIT_LINK_HANDOVER_COMPLETE   MIH_C_BIT_6_VALUE
#define MIH_C_BIT_LINK_PDU_TRANSMIT_STATUS MIH_C_BIT_7_VALUE
/*! \var MIH_C_LINK_EVENT_LIST_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A list of link events. The specified event is selected if the corresponding bit is set to 1.
*        Bitmap values:
*        Bit 0: Link_Detected
*        Bit 1: Link_Up
*        Bit 2: Link_Down
*        Bit 3: Link_Parameters_Report
*        Bit 4: Link_Going_Down
*        Bit 5: Link_Handover_Imminent
*        Bit 6: Link_Handover_Complete
*        Bit 7: Link_PDU_Transmit_Status
*        Bit 8–31: (Reserved)
*/
TYPEDEF_BITMAP32(MIH_C_LINK_EVENT_LIST)
//-------------------------------------------
/*! \var MIH_C_LINK_GD_REASON_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents the reason of a link going down. See Table F.8 for the enumeration values.
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_LINK_GD_REASON)
//-------------------------------------------
#define MIH_C_BIT_EVENT_SERVICE_SUPPORTED        MIH_C_BIT_1_VALUE
#define MIH_C_BIT_COMMAND_SERVICE_SUPPORTED      MIH_C_BIT_2_VALUE
#define MIH_C_BIT_INFORMATION_SERVICE_SUPPORTED  MIH_C_BIT_3_VALUE
/*! \var MIH_C_LINK_MIHCAP_FLAG_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents if MIH capability is supported or not. If the bit is set, it indicates
*        that the capability is supported.
*        Bitmap values:
*        Bit 1: event service (ES) supported
*        Bit 2: command service (CS) supported
*        Bit 3: information service (IS) supported
*        Bit 0, 4–7: (Reserved)
*/
TYPEDEF_BITMAP8(MIH_C_LINK_MIHCAP_FLAG)
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_VAL_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief The current value of the parameter. The format of the media-dependent value is defined in
*        the respective media specification standard and the equivalent number of bits (i.e., first
*        bits) of this data type is used. In case that there are remaining unused bits in the data
*        type, these are marked as all-zeros (‘0’).
*        Valid Range: 0..65535
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_LINK_PARAM_VAL)





//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.9 Data types for QOS (MAINLY because of precedence definition)
//-----------------------------------------------------------------------------
//-------------------------------------------
/*! \var MIH_C_COS_ID_T
* \ingroup MIH_C_F9_DATA_TYPES_FOR_QOS
* \brief Defines MIH_C_COS_ID_T as a MIH_C_UNSIGNED_INT1_T.
*        Valid Range: 0–255
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_COS_ID)
//-------------------------------------------
/*! \struct  MIH_C_MIN_PK_TX_DELAY_T
* \ingroup MIH_C_F9_DATA_TYPES_FOR_QOS
* \brief A type to represent the minimum packet transfer delay in ms for the specific CoS specified by the COS_ID.
*/
typedef struct MIH_C_MIN_PK_TX_DELAY {
    MIH_C_COS_ID_T               cos_id;
    UNSIGNED_INT2(value)
} MIH_C_MIN_PK_TX_DELAY_T;
//-------------------------------------------
/*! \struct  MIH_C_AVG_PK_TX_DELAY_T
* \ingroup MIH_C_F9_DATA_TYPES_FOR_QOS
* \brief A type to represent the average packet transfer delay in ms for the specific CoS specified by the COS_ID.
*/
typedef struct MIH_C_AVG_PK_TX_DELAY {
    MIH_C_COS_ID_T               cos_id;
    UNSIGNED_INT2(value)
} MIH_C_AVG_PK_TX_DELAY_T;
//-------------------------------------------
/*! \struct  MIH_C_MAX_PK_TX_DELAY_T
* \ingroup MIH_C_F9_DATA_TYPES_FOR_QOS
* \brief A type to represent the maximum packet transfer delay in ms for the specific CoS specified by the COS_ID.
*/
typedef struct MIH_C_MAX_PK_TX_DELAY {
    MIH_C_COS_ID_T               cos_id;
    UNSIGNED_INT2(value)
} MIH_C_MAX_PK_TX_DELAY_T;
//-------------------------------------------
/*! \struct  MIH_C_PK_DELAY_JITTER_T
* \ingroup MIH_C_F9_DATA_TYPES_FOR_QOS
* \brief A type to represent the packet transfer delay jitter in ms for the specific CoS specified by the COS_ID.
*/
typedef struct MIH_C_PK_DELAY_JITTER {
    MIH_C_COS_ID_T               cos_id;
    UNSIGNED_INT2(value)
} MIH_C_PK_DELAY_JITTER_T;
//-------------------------------------------
/*! \struct  MIH_C_PK_LOSS_RATE_T
* \ingroup MIH_C_F9_DATA_TYPES_FOR_QOS
* \brief A type to represent the packet loss rate for the specific CoS specified by the COS_ID. The loss rate is
*        equal to the integer part of the result of multiplying –100 times the log10 of the ratio between the num-
*        ber of packets lost and the total number of packets transmitted in the class population of interest.
*/
typedef struct MIH_C_PK_LOSS_RATE {
    MIH_C_COS_ID_T               cos_id;
    UNSIGNED_INT2(value)
} MIH_C_PK_LOSS_RATE_T;
//-------------------------------------------
/*! \var MIH_C_MIN_PK_TX_DELAY_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_MIN_PK_TX_DELAY, MIH_C_QOS_LIST_LENGTH)
/*! \var MIH_C_AVG_PK_TX_DELAY_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_AVG_PK_TX_DELAY, MIH_C_QOS_LIST_LENGTH)
/*! \var MIH_C_MAX_PK_TX_DELAY_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_MAX_PK_TX_DELAY, MIH_C_QOS_LIST_LENGTH)
/*! \var MIH_C_PK_DELAY_JITTER_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_PK_DELAY_JITTER, MIH_C_QOS_LIST_LENGTH)
/*! \var MIH_C_PK_LOSS_RATE_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_PK_LOSS_RATE,    MIH_C_QOS_LIST_LENGTH)

//-------------------------------------------
/*! \var MIH_C_NUM_COS_TYPES_T
* \ingroup MIH_C_F9_DATA_TYPES_FOR_QOS
* \brief
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_NUM_COS_TYPES)

/*! \struct  MIH_C_QOS_PARAM_VAL_T
* \ingroup MIH_C_F9_DATA_TYPES_FOR_QOS
* \brief A choice of Class of Service (CoS) parameters.
*/
typedef struct MIH_C_QOS_PARAM_VAL {
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_NUM_COS_TYPES_T    num_qos_types;
        LIST(MIH_C_MIN_PK_TX_DELAY, min_pk_tx_delay)
        LIST(MIH_C_AVG_PK_TX_DELAY, avg_pk_tx_delay)
        LIST(MIH_C_MAX_PK_TX_DELAY, max_pk_tx_delay)
        LIST(MIH_C_PK_DELAY_JITTER, pk_delay_jitter)
        LIST(MIH_C_PK_LOSS_RATE,    pk_loss_rate)
    } _union;
} MIH_C_QOS_PARAM_VAL_T;
#define MIH_C_QOS_PARAM_VAL_CHOICE_NUM_QOS_TYPES    (MIH_C_CHOICE_T)0
#define MIH_C_QOS_PARAM_VAL_CHOICE_MIN_PK_TX_DELAY  (MIH_C_CHOICE_T)1
#define MIH_C_QOS_PARAM_VAL_CHOICE_AVG_PK_TX_DELAY  (MIH_C_CHOICE_T)2
#define MIH_C_QOS_PARAM_VAL_CHOICE_MAX_PK_TX_DELAY  (MIH_C_CHOICE_T)3
#define MIH_C_QOS_PARAM_VAL_CHOICE_PK_DELAY_JITTER  (MIH_C_CHOICE_T)4
#define MIH_C_QOS_PARAM_VAL_CHOICE_PK_LOSS_RATE     (MIH_C_CHOICE_T)5

//-------------------------------------------
/*! \struct  MIH_C_QOS_LIST_T
* \ingroup MIH_C_F9_DATA_TYPES_FOR_QOS
* \brief A list of Class of Service (CoS) parameters.
*/
typedef struct MIH_C_QOS_LIST {
    MIH_C_NUM_COS_TYPES_T              num_qos_types;
    LIST(MIH_C_MIN_PK_TX_DELAY, min_pk_tx_delay)
    LIST(MIH_C_AVG_PK_TX_DELAY, avg_pk_tx_delay)
    LIST(MIH_C_MAX_PK_TX_DELAY, max_pk_tx_delay)
    LIST(MIH_C_PK_DELAY_JITTER, pk_delay_jitter)
    LIST(MIH_C_PK_LOSS_RATE,    pk_loss_rate)
} MIH_C_QOS_LIST_T;



//-------------------------------------------
/*! \struct  MIH_C_LINK_PARAM_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents a link parameter type and value pair.
*/
typedef struct MIH_C_LINK_PARAM {
    MIH_C_LINK_PARAM_TYPE_T      link_param_type;
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_LINK_PARAM_VAL_T   link_param_val;
        MIH_C_QOS_PARAM_VAL_T    qos_param_val;
    } _union;
} MIH_C_LINK_PARAM_T;
#define MIH_C_LINK_PARAM_CHOICE_LINK_PARAM_VAL (MIH_C_CHOICE_T)0
#define MIH_C_LINK_PARAM_CHOICE_QOS_PARAM_VAL  (MIH_C_CHOICE_T)1
//-------------------------------------------
/*! \struct  MIH_C_LINK_PARAM_RPT_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents a link parameter report.
*        Includes an option of the THRESHOLD that was crossed.
*        If no THRESHOLD is included, then this is a periodic report.
*/
typedef struct MIH_C_LINK_PARAM_RPT {
    MIH_C_LINK_PARAM_T           link_param;
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_THRESHOLD_T        threshold;
    } _union;
} MIH_C_LINK_PARAM_RPT_T;
#define MIH_C_LINK_PARAM_RPT_CHOICE_NULL      0
#define MIH_C_LINK_PARAM_RPT_CHOICE_THRESHOLD 1
//-------------------------------------------
/*! \var MIH_C_LINK_ADDR_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_LINK_ADDR, MIH_C_LINK_ADDR_LIST_LENGTH)
/*! \struct  MIH_C_LINK_POA_LIST_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A list of PoAs for a particular link. The LIST(LINK_ADDR) is a list of PoA
*        link addresses and is sorted from most preferred first to least preferred last.
*/
typedef struct MIH_C_LINK_POA_LIST {
    MIH_C_LINK_ID_T              link_id;
    LIST(MIH_C_LINK_ADDR, link_addr)
} MIH_C_LINK_POA_LIST_T;
//-------------------------------------------
#define MIH_C_LINK_RES_STATUS_AVAILABLE          MIH_C_MIH_C_BOOLEAN_TRUE
#define MIH_C_LINK_RES_STATUS_NOT_AVAILABLE      MIH_C_BOOLEAN_FALSE
/*! \var MIH_C_LINK_RES_STATUS_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Indicates if a resource is available or not.
*        TRUE: Available
*        FALSE: Not available.
*/
TYPEDEF_BOOLEAN(MIH_C_LINK_RES_STATUS)
//-------------------------------------------
#define MIH_C_BIT_LINK_STATES_REQ_OP_MODE            MIH_C_BIT_0_VALUE
#define MIH_C_BIT_LINK_STATES_REQ_CHANNEL_ID         MIH_C_BIT_1_VALUE
/*! \var MIH_C_LINK_STATES_REQ_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Link states to be requested.
* Bit 0: OP_MODE
* Bit 1: CHANNEL_ID
* Bit 2–15: (Reserved)
*/
TYPEDEF_BITMAP16(MIH_C_LINK_STATES_REQ)
//-------------------------------------------
#define MIH_C_OPMODE_NORMAL_MODE                 0
#define MIH_C_OPMODE_POWER_SAVING_MODE           1
#define MIH_C_OPMODE_POWERED_DOWN                2
/*! \var MIH_C_OPMODE_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief The link power mode.
*        0: Normal Mode
*        1: Power Saving Mode
*        2: Powered Down
*        3–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_OPMODE)
//-------------------------------------------
/*! \struct  MIH_C_LINK_STATES_RSP_T
* \brief The operation mode or the channel ID of the link.
*/
typedef struct MIH_C_LINK_STATES_RSP {
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_OPMODE_T           op_mode;
        MIH_C_CHANNEL_ID_T       channel_id;
    } _union;
} MIH_C_LINK_STATES_RSP_T;
//-------------------------------------------
/*! \var MIH_C_LINK_PARAM_TYPE_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_LINK_PARAM_TYPE, MIH_C_LINK_STATUS_REQ_LIST_LENGTH)
/*! \struct MIH_C_LINK_STATUS_REQ_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief Represents the possible information to request from a link.
*/
typedef struct MIH_C_LINK_STATUS_REQ {
    MIH_C_LINK_STATES_REQ_T      link_states_req;
    LIST(MIH_C_LINK_PARAM_TYPE, link_param_type)
    MIH_C_LINK_DESC_RSP_T        link_desc_rsp;
} MIH_C_LINK_STATUS_REQ_T;
//-------------------------------------------
/*! \var MIH_C_LINK_STATES_RSP_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_LINK_STATES_RSP, MIH_C_LINK_STATUS_REQ_LIST_LENGTH)
/*! \var MIH_C_LINK_PARAM_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_LINK_PARAM, MIH_C_LINK_STATUS_REQ_LIST_LENGTH)
/*! \var MIH_C_LINK_DESC_RSP_LIST_T
* \brief
*/
TYPEDEF_LIST(MIH_C_LINK_DESC_RSP, MIH_C_LINK_STATUS_REQ_LIST_LENGTH)
/*! \struct  MIH_C_LINK_STATUS_RSP_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief A set of link status parameter values correspond to the LINK_STATUS_REQ.
*/
typedef struct MIH_C_LINK_STATUS_RSP {
    LIST(MIH_C_LINK_STATES_RSP,  link_states_rsp)
    LIST(MIH_C_LINK_PARAM,       link_param)
    LIST(MIH_C_LINK_DESC_RSP,    link_desc_rsp)
} MIH_C_LINK_STATUS_RSP_T;
//-------------------------------------------
/*! \struct  MIH_C_LINK_TUPLE_ID_T
* \ingroup MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC
* \brief The identifier of a link that is associated with a PoA. The LINK_ID contains the MN LINK_ADDR.
*        The optional LINK_ADDR contains a link address of PoA.
*/
typedef struct MIH_C_LINK_TUPLE_ID {
    MIH_C_LINK_ID_T              link_id;
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_LINK_ADDR_T        link_addr;
    } _union;
} MIH_C_LINK_TUPLE_ID_T;
#define MIH_C_LINK_TUPLE_ID_CHOICE_NULL      0
#define MIH_C_LINK_TUPLE_ID_CHOICE_LINK_ADDR 1
//-------------------------------------------




//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.5 Link actions
//-----------------------------------------------------------------------------
#define MIH_C_LINK_ACTION_LINK_NONE                             0
#define MIH_C_LINK_ACTION_LINK_LINK_DISCONNECT                  1  /* Disconnect the link connection directly.*/
#define MIH_C_LINK_ACTION_LINK_LOW_POWER                        2  /* Cause the link to adjust its battery power level to be low power consumption. */
#define MIH_C_LINK_ACTION_LINK_POWER_DOWN                       3  /* Cause the link to power down and turn off the radio.*/
#define MIH_C_LINK_ACTION_LINK_POWER_UP                         4  /* Cause the link to power up and establish L2 connectivity. For UMTS link type, power up lower layers and establish PDP context. */




//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.6 Link action attibutes
//-----------------------------------------------------------------------------
#define MIH_C_LINK_ACTION_ATTRIBUTE_DATA_FWD_REQ                0 /* This indication requires   the buffered data at the old serving PoA entity to be forwarded
                                                                   * to the new target PoA entity in order to avoid data loss. This action can be taken imme-
                                                                   * diately after the old serving PoS receives MIH_N2N_HO_Commit response message from the new
                                                                   * target PoS, or the old serving PoS receives MIH_Net_HO_Commit response message from the MN.
                                                                   * This is not valid on UMTS link type.*/
#define MIH_C_LINK_ACTION_ATTRIBUTE_LINK_RES_RETAIN             1 /* The link will be disconnected but the resource for the link connection still remains so
                                                                   * reestablishing the link connection later can be more efficient.*/
#define MIH_C_LINK_ACTION_ATTRIBUTE_LINK_SCAN                   2 /* Cause the link to perform a scan.*/




//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.7 Link down reason code
//-----------------------------------------------------------------------------
#define MIH_C_LINK_DOWN_REASON_EXPLICIT_DISCONNECT              0 /* The link is down because of explicit disconnect procedures initiated either by MN or network.*/
#define MIH_C_LINK_DOWN_REASON_PACKET_TIMEOUT                   1 /* The link is down because no acknowledgements were received for transmitted packets within the
                                                                   * specified time limit.*/
#define MIH_C_LINK_DOWN_REASON_NO_RESOURCE                      2 /* The link is down because there were no resources to maintain the connection.*/
#define MIH_C_LINK_DOWN_REASON_NO_BROADCAST                     3 /* The link is down because broadcast messages (such as beacons in IEEE 802.11 management frames)
                                                                   * could not be received by MN. */
#define MIH_C_LINK_DOWN_REASON_AUTHENTICATION_FAILURE           4 /* Authentication failure.*/
#define MIH_C_LINK_DOWN_REASON_BILLING_FAILURE                  5 /* Billing failure.*/




//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.8 Link going down reason code
//-----------------------------------------------------------------------------
#define MIH_C_LINK_GOING_DOWN_REASON_EXPLICIT_DISCONNECT        0 /* The link is going to be down because explicit disconnect procedures will be
                                                                   * initiated either by MN or network. For example, when a BS has decided to
                                                                   * shutdown for administrative reasons or an operator of the terminal has
                                                                   * decided to execute a handover manually, a Link_Going_Down trigger is sent
                                                                   * to the MIHF.
                                                                   */
#define MIH_C_LINK_GOING_DOWN_REASON_LINK_PARAMETER_DEGRADING   1 /* The link is going to be down because broadcast messages (such as beacons in
                                                                   * IEEE 802.11 management frames) could not be received by MN.
                                                                   */
#define MIH_C_LINK_GOING_DOWN_REASON_LOW_POWER                  2 /* The link is going to be down because the power level of the terminal is low
                                                                   * and the current link will not be maintained in such a low power level. Mobile
                                                                   * terminals usually have limited battery supply, and when the battery level of
                                                                   * the terminal is low, a terminal can choose a link that has lower power con-
                                                                   * sumption for handover according to the received Link_Going_Down triggers
                                                                   * with this reason code. This will lengthen the usable time for the terminal.
                                                                   */
#define MIH_C_LINK_GOING_DOWN_REASON_NO_RESOURCE                3 /* The link is going to be down because there will be no resources to maintain
                                                                   * the current connection. For example, a BS that has too many users can send
                                                                   * Link_Going_Down indications to terminals when the links with them can not
                                                                   * be kept because of insufficient resources. Another example is that users with
                                                                   * higher priority can preempt the ones with lower priority when no more
                                                                   * resources can be allocated in 3GPP, and this can also cause a
                                                                   * Link_Going_Down indication with this reason code.
                                                                   */


//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.10 Not implemented
//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.11 Not implemented
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.12 Data types for IP configuration
//-----------------------------------------------------------------------------
#define MIH_C_BIT_IP_CFG_MTHDS_IPV4_STATIC_CONFIGURATION             MIH_C_BIT_0_VALUE
#define MIH_C_BIT_IP_CFG_MTHDS_IPV4_DYNAMIC_CONFIGURATION            MIH_C_BIT_1_VALUE
#define MIH_C_BIT_IP_CFG_MTHDS_MOBILE_IPV4_WITH_FA_COA               MIH_C_BIT_2_VALUE
#define MIH_C_BIT_IP_CFG_MTHDS_MOBILE_IPV4_WITHOUT_FA                MIH_C_BIT_3_VALUE
#define MIH_C_BIT_IP_CFG_MTHDS_IPV6_STATELESS_ADDRESS_CONFIGURATION  MIH_C_BIT_11_VALUE
#define MIH_C_BIT_IP_CFG_MTHDS_IPV6_STATEFULL_ADDRESS_CONFIGURATION  MIH_C_BIT_12_VALUE
#define MIH_C_BIT_IP_CFG_MTHDS_IPV6_MANUAL_ADDRESS_CONFIGURATION     MIH_C_BIT_13_VALUE
/*! \var MIH_C_IP_CFG_MTHDS_T
* \brief A set of IP configuration methods.
*         Bit 0: IPv4 static configuration
*         Bit 1: IPv4 dynamic configuration (DHCPv4)
*         Bit 2: Mobile IPv4 with foreign agent (FA) care-of address (CoA) (FA-CoA)
*         Bit 3: Mobile IPv4 without FA (Co-located CoA)
*         Bits 4–10: reserved for IPv4 address configurations
*         Bit 11: IPv6 stateless address configuration
*         Bit 12: IPv6 stateful address configuration (DHCPv6)
*         Bit 13: IPv6 manual configuration
*         Bits 14–31: (Reserved)
*/
TYPEDEF_BITMAP32(MIH_C_IP_CFG_MTHDS)
//-------------------------------------------
#define MIH_C_BIT_IP_MOB_MGMT_MOBILE_IPV4                             MIH_C_BIT_0_VALUE
#define MIH_C_BIT_IP_MOB_MGMT_MOBILE_IPV4_REGIONAL_REGISTRATION       MIH_C_BIT_1_VALUE
#define MIH_C_BIT_IP_MOB_MGMT_MOBILE_IPV6                             MIH_C_BIT_2_VALUE
#define MIH_C_BIT_IP_MOB_MGMT_HIERARCHICAL_MOBILE_IPV6                MIH_C_BIT_3_VALUE
#define MIH_C_BIT_IP_MOB_MGMT_LOW_LATENCY_HANDOFFS                    MIH_C_BIT_4_VALUE
#define MIH_C_BIT_IP_MOB_MGMT_MOBILE_IPV6_FAST_HANDOVERS              MIH_C_BIT_5_VALUE
#define MIH_C_BIT_IP_MOB_MGMT_IKEV2_MOBILITY_AND_MULTIHOMING_PROTOCOL MIH_C_BIT_6_VALUE
/*! \var MIH_C_IP_MOB_MGMT_T
* \brief Indicates the supported mobility management protocols.
*         Bit 0: Mobile IPv4 (IETF RFC 3344)
*         Bit 1: Mobile IPv4 Regional Registration (IETF RFC 4857)
*         Bit 2: Mobile IPv6 (IETF RFC 3775)
*         Bit 3: Hierarchical Mobile IPv6 (IETF RFC 4140)
*         Bit 4: Low Latency Handoffs (IETF RFC 4881)
*         Bit 5: Mobile IPv6 Fast Handovers (IETF RFC 5268)
*         Bit 6: IKEv2 Mobility and Multihoming Protocol (IETF RFC 4555)
*         Bit 7–15: (Reserved)
*/
TYPEDEF_BITMAP16(MIH_C_IP_MOB_MGMT)
//-------------------------------------------
/*! \var MIH_C_IP_PREFIX_LEN_T
* \brief The length of an IP subnet prefix.
*         Valid Range:
*         0..32 for IPv4 subnet.
*         0..64, 65..127 for IPv6 subnet. (IETF RFC 4291 [B25])
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_IP_PREFIX_LEN)
//-------------------------------------------
/*! \var MIH_C_IP_RENEWAL_FLAG_T
* \brief Indicates whether MN’s IP address needs to be changed or not.
*         TRUE: Change required.
*         FALSE: Change not required.
*/
TYPEDEF_BOOLEAN(MIH_C_IP_RENEWAL_FLAG)
//-------------------------------------------
/*! \struct  MIH_C_IP_SUBNET_INFO_T
* \brief Represent an IP subnet. The IP_PREFIX_LEN contains the bit
*         length of the prefix of the subnet to which the IP_ADDR
*         belongs.
*/
typedef struct MIH_C_IP_SUBNET_INFO {
    MIH_C_IP_PREFIX_LEN_T              ip_prefix_len;
    MIH_C_IP_ADDR_T                    ip_addr;
} MIH_C_IP_SUBNET_INFO_T;




//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.13 Data types for information elements
//-----------------------------------------------------------------------------
/*! \var  MIH_C_NET_AUX_ID_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent an auxiliary access network
*        identifier. This is HESSID if network type is
*        IEEE 802.11.
*/
TYPEDEF_OCTET_STRING(MIH_C_NET_AUX_ID, 253)

// already defined in other section TYPEDEF_OCTET_STRING(MIH_C_NETWORK_ID, 253)
/*! \var MIH_C_BAND_CLASS_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief CDMA band class.
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_BAND_CLASS)
/*! \var MIH_C_BANDWIDTH_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief Channel bandwidth in kb/s.
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_BANDWIDTH)
/*! \var MIH_C_BASE_ID_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief Base station identifier.
*/
TYPEDEF_UNSIGNED_INT2(MIH_C_BASE_ID)
/*! \var MIH_C_DOWN_BP_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A List of FEC Code Type for Downlink burst.
*        Refer to 11.4.1 in IEEE 802.16Rev2/D5.0.
*/
TYPEDEF_BITMAP256(MIH_C_DOWN_BP)
/*! \var  MIH_C_TYPE_EXT_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A generic type extension contained indicating a flexible length and format field. The content is to be
*        defined and filled by the appropriate SDO or service provider consortium, etc.
*        The value is a non-NULL terminated string whose length shall not exceed 253 octets.
*/
TYPEDEF_OCTET_STRING(MIH_C_TYPE_EXT, 253)
/*! \var MIH_C_UP_BP_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A List of FEC Code Type for Uplink burst.
*        Refer to 11.3.1 in IEEE 802.16Rev2/D5.0
*/
TYPEDEF_BITMAP256(MIH_C_UP_BP)
//-------------------------------------------
/*! \struct  MIH_C_BURST_PROF_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief Burst profile.
*/
typedef struct MIH_C_BURST_PROF {
    MIH_C_DOWN_BP_T              down_bp;
    MIH_C_UP_BP_T                up_bp;
} MIH_C_BURST_PROF_T;
//-------------------------------------------
/*! \struct  MIH_C_CH_RANGE_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type that contains two numbers. The first unsigned
*        integer is the low range. The second unsigned integer
*        is the high range. Both values are in kHz.
*        The first unsigned integer value should always be less
*        than or equal to the second unsigned integer.
*/
typedef struct MIH_C_CH_RANGE {
    UNSIGNED_INT4(low_range);
    UNSIGNED_INT4(high_range);
} MIH_C_CH_RANGE_T;
//-------------------------------------------
#define MIH_C_COST_UNIT_SECOND    0
#define MIH_C_COST_UNIT_MINUTE    1
#define MIH_C_COST_UNIT_HOURS     2
#define MIH_C_COST_UNIT_DAY       3
#define MIH_C_COST_UNIT_WEEK      4
#define MIH_C_COST_UNIT_MONTH     5
#define MIH_C_COST_UNIT_YEAR      6
#define MIH_C_COST_UNIT_FREE      7
#define MIH_C_COST_UNIT_FLAT_RATE 8
/*! \var MIH_C_COST_UNIT_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent the unit of a cost.
*        0: second
*        1: minute
*        2: hours
*        3: day
*        4: week
*        5: month
*        6: year
*        7: free
*        8: flat rate
*        9–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_COST_UNIT)
//-------------------------------------------
/*! \struct  MIH_C_COST_VALUE_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent the value of a cost.
*        The first 4-octet contains the integer part of the cost.
*        The last 2-octet contains the fraction part where it rep-
*        resents a 3-digit fraction.
*        Therefore, the value range of the fraction part is
*        [0,999].
*        For example, for a value of “0.5”, the integer part is
*             zero and the fraction part is 500.
*/
typedef struct MIH_C_COST_VALUE {
    UNSIGNED_INT4(cost_integer_part);
    UNSIGNED_INT2(cost_fraction_part);
} MIH_C_COST_VALUE_T;
//-------------------------------------------
/*! \var  MIH_C_COST_CURR_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent the currency of a cost.
*        A three-letter currency code (e.g., “USD”) specified
*        by ISO 4217.
*/
TYPEDEF_OCTET_STRING(MIH_C_COST_CURR, 3)
//-------------------------------------------
/*! \struct  MIH_C_COST_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent a cost.
*/
typedef struct MIH_C_COST {
    MIH_C_COST_UNIT_T              cost_unit;
    MIH_C_COST_VALUE_T             cost_value;
    MIH_C_COST_CURR_T              cost_curr;
} MIH_C_COST_T;
//-------------------------------------------
/*! \var MIH_C_CNTRY_CODE_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief Country code, represented as two letter ISO 3166-1 country code in capital ASCII letters.
*/
TYPEDEF_OCTET(MIH_C_CNTRY_CODE, 2)
//-------------------------------------------
/*! \var MIH_C_DATA_RATE_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief
*/
TYPEDEF_UNSIGNED_INT4(MIH_C_DATA_RATE)
//-------------------------------------------
/*! \var MIH_C_DU_CTR_FREQ_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief Downlink/Uplink center frequency in kHz.
*/
TYPEDEF_INTEGER8(MIH_C_DU_CTR_FREQ)
//-------------------------------------------
/*! \var MIH_C_EIRP_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief BS’s effective isotropic radiated power level. Signed in units of 1 dBm.
*/
TYPEDEF_INTEGER4(MIH_C_EIRP)
//-------------------------------------------
/*! \struct  MIH_C_GAP_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief This gap is an integer number of physical slot durations and starts on a
*        physical slot boundary. Used on TDD systems only.
*        The UNSIGNED_INT(2) is used for the TTG - transmit/receive transition gap.
*        The UNSIGNED_INT(1) is used for the RTG - receive/transmit transition gap.
*/
typedef struct MIH_C_GAP
 {
    UNSIGNED_INT2(ttg);
    UNSIGNED_INT1(rtg);
} MIH_C_GAP_T;
//-------------------------------------------
/*! \var MIH_C_HO_CODE_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief HANDOVER_RANGING_CODE.
*        Refer to 11.3.1 in IEEE 802.16Rev2/D5.0.
*/
TYPEDEF_INTEGER1(MIH_C_HO_CODE)
//-------------------------------------------
/*! \var MIH_C_INIT_CODE_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief INITIAL_RANGING_CODE.
*        Refer to 11.3.1 in IEEE 802.16Rev2/D5.0.
*/
TYPEDEF_INTEGER1(MIH_C_INIT_CODE)
//-------------------------------------------
/*! \struct  MIH_C_CDMA_CODES_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A set of CDMA ranging codes.
*/
typedef struct MIH_C_CDMA_CODES {
    MIH_C_INIT_CODE_T            init_code;
    MIH_C_HO_CODE_T              ho_code;
} MIH_C_CDMA_CODES_T;
//-------------------------------------------
/*! \struct  MIH_C_DCD_UCD_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent the downlink channel descriptor and the uplink channel descriptor.
*/
typedef struct MIH_C_DCD_UCD {
    MIH_C_BASE_ID_T          base_id;
    MIH_C_BANDWIDTH_T        bandwidth;
    MIH_C_DU_CTR_FREQ_T      du_ctr_freq;
    MIH_C_EIRP_T             eirp;
    MIH_C_GAP_T              gap;
    MIH_C_BURST_PROF_T       burst_prof;
    MIH_C_CDMA_CODES_T       cdma_codes;
} MIH_C_DCD_UCD_T;
//-------------------------------------------
/*! \var  MIH_C_FQDN_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief The fully qualified domain name of a host as described in IETF RFC 2181.
*/
TYPEDEF_OCTET_STRING(MIH_C_FQDN, 253)
//-------------------------------------------
// TBD FREQ_BANDS
/*! \var MIH_C_FREQ_ID_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief Identifier of the carrier frequency. Valid Range: 0..65535
*/
TYPEDEF_INTEGER2(MIH_C_FREQ_ID)
//-------------------------------------------
/*! \var MIH_C_FQ_CODE_NUM_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief UMTS scrambling code, cdma2000 Walsh code.
*        Valid Range: 0..65535
*/
TYPEDEF_INTEGER2(MIH_C_FQ_CODE_NUM)
//-------------------------------------------
/*! \var  MIH_C_IP4_ADDR_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief An IPv4 address as described in IETF RFC 791
*/
TYPEDEF_OCTET(MIH_C_IP4_ADDR, 4)
/*! \var  MIH_C_IP6_ADDR_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief An IPv6 address as described in IETF RFC 4291
*/
TYPEDEF_OCTET(MIH_C_IP6_ADDR, 16)

/*! \var MIH_C_DHCP_SERV_T
* \ingroup MIH_C_F22_DATA_TYPE_FOR_HANDOVER_OPERATION
* \brief IP address of candidate DHCP Server. It is only
* included when dynamic address configuration is supported.
*/
typedef MIH_C_IP_ADDR_T MIH_C_DHCP_SERV_T;
/*! \var MIH_C_FN_AGNT_T
* \ingroup MIH_C_F22_DATA_TYPE_FOR_HANDOVER_OPERATION
* \brief IP address of candidate Foreign Agent. It is only included when Mobile IPv4 is supported.
*/
typedef MIH_C_IP_ADDR_T MIH_C_FN_AGNT_T;
/*! \var MIH_C_ACC_RTR_T
* \ingroup MIH_C_F22_DATA_TYPE_FOR_HANDOVER_OPERATION
* \brief IP address of candidate Access Router. It is only
* included when IPv6 Stateless configuration is supported.
*/
typedef MIH_C_IP_ADDR_T MIH_C_ACC_RTR_T;
//-------------------------------------------
/*! \struct  MIH_C_IP_CONFIG_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief IP Configuration Methods supported by the access network.
*/
typedef struct MIH_C_IP_CONFIG
 {
    MIH_C_IP_CFG_MTHDS_T         ip_cfg_mthds;
    MIH_C_CHOICE_T               choice_dhcp_serv;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_DHCP_SERV_T        dhcp_serv;
    } _union_dhcp_serv;
    MIH_C_CHOICE_T               choice_fn_agnt;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_FN_AGNT_T          fn_agnt;
    } _union_fn_agnt;
    MIH_C_CHOICE_T               choice_acc_rtr;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_ACC_RTR_T          acc_rtr;
    } _union_acc_rtr;
} MIH_C_IP_CONFIG_T;
//-------------------------------------------
#define MIH_C_BIT_NET_CAPS_SECURITY                MIH_C_BIT_0_VALUE
#define MIH_C_BIT_NET_CAPS_QOS_CLASS0              MIH_C_BIT_1_VALUE
#define MIH_C_BIT_NET_CAPS_QOS_CLASS1              MIH_C_BIT_2_VALUE
#define MIH_C_BIT_NET_CAPS_QOS_CLASS2              MIH_C_BIT_3_VALUE
#define MIH_C_BIT_NET_CAPS_QOS_CLASS3              MIH_C_BIT_4_VALUE
#define MIH_C_BIT_NET_CAPS_QOS_CLASS4              MIH_C_BIT_5_VALUE
#define MIH_C_BIT_NET_CAPS_QOS_CLASS5              MIH_C_BIT_6_VALUE
#define MIH_C_BIT_NET_CAPS_INTERNET_ACCESS         MIH_C_BIT_7_VALUE
#define MIH_C_BIT_NET_CAPS_EMERGENCY_SERVICES      MIH_C_BIT_8_VALUE
#define MIH_C_BIT_NET_CAPS_MIH_CAPABILITY          MIH_C_BIT_9_VALUE
/*! \var MIH_C_NET_CAPS_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief These bits provide high level capabilities supported on a network.
*        Bitmap Values:
*        Bit 0: Security – Indicates that some level of security is supported when set.
*        Bit 1: QoS Class 0 – Indicates that QoS for class 0 is supported when set.
*        Bit 2: QoS Class 1 – Indicates that QoS for class 1 is supported when set.
*        Bit 3: QoS Class 2 – Indicates that QoS for class 2 is supported when set; Otherwise, no QoS for class 2 support is available.
*        Bit 4: QoS Class 3 – Indicates that QoS for class 3 is supported when set; Otherwise, no QoS for class 3 support is available.
*        Bit 5: QoS Class 4 – Indicates that QoS for class 4 is supported when set; Otherwise, no QoS for class 4 support is available.
*        Bit 6: QoS Class 5 – Indicates that QoS for class 5 is supported when set; Otherwise, no QoS for class 5 support is available.
*        Bit 7: Internet Access – Indicates that Internet access is supported when set; Otherwise, no Internet access support is available.
*        Bit 8: Emergency Services – Indicates that some level of emergency services is supported when set; Otherwise, no emergency service support is available.
*        Bit 9: MIH Capability – Indicates that MIH is supported when set; Otherwise, no MIH support is available.
*        Bit 10–31: (Reserved)
*/
TYPEDEF_BITMAP32(MIH_C_NET_CAPS)
//-------------------------------------------
/*! \var MIH_C_SUBTYPE_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A network subtype. See Table F.14.
*/
TYPEDEF_BITMAP64(MIH_C_SUBTYPE)
//-------------------------------------------

//-------------------------------------------
/*! \struct  MIH_C_NETWORK_TYPE_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent a network type and its subtype.
*        See Table F.14 for details.
*/
typedef struct MIH_C_NETWORK_TYPE
 {
    MIH_C_CHOICE_T               choice_link_type;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_LINK_TYPE_T        link_type;
    } _union_link_type;
    MIH_C_CHOICE_T               choice_subtype;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_SUBTYPE_T          subtype;
    } _union_subtype;
    MIH_C_CHOICE_T               choice_type_ext;
    union  {
        MIH_C_NULL_T             null_attr;
        MIH_C_TYPE_EXT_T         type_ext;
    } _union_type_ext;
} MIH_C_NETWORK_TYPE_T;
//-------------------------------------------
/*! \var  MIH_C_OP_NAME_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent an operator name. The value
*        uniquely identifies the operator name within the scope
*        of the OP_NAMESPACE.
*        The value is a non NULL terminated string whose
*        length shall not exceed 253 octets.
*/
TYPEDEF_OCTET_STRING(MIH_C_OP_NAME, 253)
//-------------------------------------------
/*! \var MIH_C_OP_NAMESPACE_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent a type of operator name.
*        0: GSM/UMTS
*        1: CDMA
*        2: REALM (as defined in [B28]).
*        3: ITU-T/TSB
*        4: General
*        5–255: (Reserved)
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_OP_NAMESPACE)
//-------------------------------------------
/*! \struct  MIH_C_OPERATOR_ID_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent an operator identifier.
*/
typedef struct MIH_C_OPERATOR_ID {
    MIH_C_OP_NAME_T               opname;
    MIH_C_OP_NAMESPACE_T          opnamespace;
} MIH_C_OPERATOR_ID_T;
//-------------------------------------------
/*! \struct  MIH_C_SIB_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A type to represent UMTS system information block (SIB).
*/
typedef struct MIH_C_SIB {
    MIH_C_CELL_ID_T               cell_id;
    MIH_C_FQ_CODE_NUM_T           fq_code_num;
} MIH_C_SIB_T;
//-------------------------------------------
/*! \var MIH_C_PILOT_PN_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief Pilot PN sequence offset index.
*/
TYPEDEF_INTEGER2(MIH_C_PILOT_PN)
//-------------------------------------------
/*! \struct  MIH_C_SYS_PARAMS_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief CDMA2000 system parameters.
*/
typedef struct MIH_C_SYS_PARAMS {
    MIH_C_BASE_ID_T               base_id;
    MIH_C_PILOT_PN_T              pilot_pn;
    MIH_C_FREQ_ID_T               freq_id;
    MIH_C_BAND_CLASS_T            band_class;
} MIH_C_SYS_PARAMS_T;
//-------------------------------------------
/*! \struct  MIH_C_MIH_C_PARAMETERS_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A data type to represent system information depending on the network type.
*        DCD_UCD: IEEE 802.16
*        SIB: UMTS
*        SYS_PARAMS: cdma2000
*/
typedef struct MIH_C_PARAMETERS {
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_DCD_UCD_T          dcd_ucd;
        MIH_C_SIB_T              sib;
        MIH_C_SYS_PARAMS_T       sys_params;
    } _union;
} MIH_C_MIH_C_PARAMETERS_T;
//-------------------------------------------
/*! \struct  MIH_C_PROXY_ADDR_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief L3 address of a proxy server.
*/
typedef struct MIH_C_PROXY_ADDR {
    MIH_C_CHOICE_T               choice;
    union  {
        MIH_C_IP4_ADDR_T         ip4_addr;
        MIH_C_IP6_ADDR_T         ip6_addr;
        MIH_C_FQDN_T             fqdn;
    } _union;
} MIH_C_PROXY_ADDR_T;
//-------------------------------------------
// TBD REGU_DOMAIN
// TBD ROAMING_PTNS
/*! \var  MIH_C_SP_ID_T
* \ingroup MIH_C_F13_DATA_TYPES_FOR_INFORMATION_ELEMENTS
* \brief A service provider identifier.
*        A non-NULL terminated string whose length shall not exceed 253 octets.
*/
TYPEDEF_OCTET_STRING(MIH_C_SP_ID, 253)
// TBD SUPPORTED_LCP
// TBD SYSTEM_INFO





//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.19 Data type for MIHF identification
//-----------------------------------------------------------------------------
/** \defgroup MIH_C_F19_DATA_TYPE_FOR_MIHF_IDENTIFICATION Data types for MIHF identification
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */
/*! \var  MIH_C_MIHF_ID_T
* \brief The MIHF Identifier: MIHF_ID is a network access identifier (NAI).
*        NAI shall be unique as per IETF RFC 4282. If L3 communication is
*        used and MIHF entity resides in the network node, then MIHF_ID is
*        the fully qualified domain name or NAI-encoded IP address
*        (IP4_ADDR or IP6_ADDR) of the entity that hosts the MIH Services.
*        If L2 communication is used then MIHF_ID is the NAI-encoded link-
*        layer address (LINK_ADDR) of the entity that hosts the MIH ser-
*        vices. In an NAI-encoded IP address or link-layer address, each octet
*        of binary-encoded IP4_ADDR, IP6_ADDR and LINK_ADDR data is
*        encoded in the username part of the NAI as “\” followed by the octet
*        value. A multicast MIHF identifier is defined as an MIHF ID of zero
*        length. When an MIH protocol message with multicast MIHF ID is
*        transmitted over the L2 data plane, a group MAC address (01-80-C2-
*        00-00-0E) shall be used (see IEEE P802.1aj/D2.2). The maximum
*        length is 253 octets.
*/
TYPEDEF_OCTET_STRING(MIH_C_MIHF_ID, 253)
/** @}*/


//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.20—Data type for MIH capabilities
//-----------------------------------------------------------------------------
/** \defgroup MIH_C_F19_DATA_TYPE_FOR_MIHF_CAPABILITIES Data type for MIH capabilities
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */
/*! \struct  MIH_C_LINK_DET_CFG_T
* \brief A data type for configuring link detected event trigger.
*/
typedef struct MIH_C_LINK_DET_CFG  {
    MIH_C_CHOICE_T                choice_network_id;
    union  {
        MIH_C_NULL_T              null_attr;
        MIH_C_NETWORK_ID_T        network_id;
    } _union_network_id;
    MIH_C_CHOICE_T                choice_sig_strength;
    union  {
        MIH_C_NULL_T              null_attr;
        MIH_C_SIG_STRENGTH_T      sig_strength;
    } _union_sig_strength;
    MIH_C_CHOICE_T                choice_link_data_rate;
    union  {
        MIH_C_NULL_T              null_attr;
        MIH_C_LINK_DATA_RATE_T    link_data_rate;
    } _union_link_data_rate;
} MIH_C_LINK_DET_CFG_T;
//-------------------------------------------
/*! \struct  MIH_C_EVT_CFG_INFO_T
* \brief Represents additional configuration information
*        for event subscription. The list of
*        LINK_DET_CFG contains additional filtering
*        when subscribing to link detected events. The list
*        of LINK_CFG_PARAM contains additional fil-
*        tering when subscribing to link parameter report
*        events.
*/
typedef struct MIH_C_EVT_CFG_INFO {
    MIH_C_CHOICE_T                choice;
    union  {
        MIH_C_LINK_DET_CFG_T      link_det_cfg;
        MIH_C_LINK_CFG_PARAM_T    link_cfg_param;
    } _union;
} MIH_C_EVT_CFG_INFO_T;
//-------------------------------------------
/*! \struct  MIH_C_LINK_DET_INFO_T
* \brief Information of a detected link.
*        LINK_TUPLE_ID is the link detected.
*        NETWORK_ID is the access network identifier.
*        NET_AUX_ID is an auxiliary access network identifier if applicable.
*        SIG_STRENGTH is the signal strength of the detected link.
*        UNSIGNED_INT(2) is the SINR value of the link.
*        LINK_DATA_RATE is the maximum transmission rate on the detected link.
*        LINK_MIHCAP_FLAG indicates which MIH capabilities are supported on the detected link.
*        NET_CAPS is the network capability supported by the network link.
*/
typedef struct MIH_C_LINK_DET_INFO {
    MIH_C_LINK_TUPLE_ID_T         link_tuple_id;
    MIH_C_NETWORK_ID_T            network_id;
    MIH_C_NET_AUX_ID_T            net_aux_id;
    MIH_C_SIG_STRENGTH_T          sig_strength;
    UNSIGNED_INT2(sinr)
    MIH_C_LINK_DATA_RATE_T        link_data_rate;
    MIH_C_LINK_MIHCAP_FLAG_T      link_mihcap_flag;
    MIH_C_NET_CAPS_T              net_caps;
} MIH_C_LINK_DET_INFO_T;
//-------------------------------------------
/*! \struct  MIH_C_MBB_HO_SUPP_T
* \brief Indicates if make before break is supported FROM the first network type TO the second network type.
*        The BOOLEAN value assignment:
*        True: Make before break is supported.
*        False: Make before break is not supported.
*/
typedef struct MIH_C_MBB_HO_SUPP {
    MIH_C_NETWORK_TYPE_T          from;
    MIH_C_NETWORK_TYPE_T          to;
    BOOLEAN(supported)
} MIH_C_MBB_HO_SUPP_T;
//-------------------------------------------
#define MIH_C_BIT_MIH_CMD_MIH_LINK_GET_PARAMETERS       MIH_C_BIT_0_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_LINK_CONFIGURE_THRESHOLDS MIH_C_BIT_1_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_LINK_ACTIONS              MIH_C_BIT_2_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_NET_HO_CANDIDATE_QUERY    MIH_C_BIT_3_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_NET_HO_COMMIT             MIH_C_BIT_3_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_N2N_HO_QUERY_RESOURCES    MIH_C_BIT_3_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_N2N_HO_COMMIT             MIH_C_BIT_3_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_N2N_HO_COMPLETE           MIH_C_BIT_3_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_MN_HO_CANDIDATE_QUERY     MIH_C_BIT_4_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_MN_HO_COMMIT              MIH_C_BIT_4_VALUE
#define MIH_C_BIT_MIH_CMD_MIH_MN_HO_COMPLETE            MIH_C_BIT_4_VALUE

/*! \var MIH_C_MIH_CMD_LIST_T
* \brief A list of MIH commands.
*        Bitmap Values:
*        Bit 0: MIH_Link_Get_Parameters
*        Bit 1: MIH_Link_Configure_Thresholds
*        Bit 2: MIH_Link_Actions
*        Bit 3: MIH_Net_HO_Candidate_Query
*        MIH_Net_HO_Commit
*        MIH_N2N_HO_Query_Resources
*        MIH_N2N_HO_Commit
*        MIH_N2N_HO_Complete
*        Bit 4: MIH_MN_HO_Candidate_Query
*        MIH_MN_HO_Commit
*        MIH_MN_HO_Complete
*        Bit 5–31: (Reserved)

*/
TYPEDEF_BITMAP32(MIH_C_MIH_CMD_LIST)

//-------------------------------------------
#define MIH_C_BIT_MIH_LINK_DETECTED                 MIH_C_BIT_0_VALUE
#define MIH_C_BIT_MIH_LINK_UP                       MIH_C_BIT_1_VALUE
#define MIH_C_BIT_MIH_LINK_DOWN                     MIH_C_BIT_2_VALUE
#define MIH_C_BIT_MIH_LINK_PARAMETERS_REPORT        MIH_C_BIT_3_VALUE
#define MIH_C_BIT_MIH_LINK_GOING_DOWN               MIH_C_BIT_4_VALUE
#define MIH_C_BIT_MIH_LINK_HANDOVER_IMMINENT        MIH_C_BIT_5_VALUE
#define MIH_C_BIT_MIH_LINK_HANDOVER_COMPLETE        MIH_C_BIT_6_VALUE
#define MIH_C_BIT_MIH_LINK_PDU_TRANSMIT_STATUS      MIH_C_BIT_7_VALUE
/*! \var MIH_C_MIH_EVT_LIST_T
* \brief A list of MIH events.
*        Bitmap Values:
*        Bit 0: MIH_Link_Detected
*        Bit 1: MIH_Link_Up
*        Bit 2: MIH_Link_Down
*        Bit 3: MIH_Link_Parameters_Report
*        Bit 4: MIH_Link_Going_Down
*        Bit 5: MIH_Link_Handover_Imminent
*        Bit 6: MIH_Link_Handover_Complete
*        Bit 7: MIH_Link_PDU_Transmit_Status
*        Bit 8–31: (Reserved)
*/
TYPEDEF_BITMAP32(MIH_C_MIH_EVT_LIST)
//-------------------------------------------
#define MIH_C_BIT_BINARY_DATA                       MIH_C_BIT_0_VALUE
#define MIH_C_BIT_RDF_DATA                          MIH_C_BIT_1_VALUE
#define MIH_C_BIT_RDF_SCHEMA_URL                    MIH_C_BIT_2_VALUE
#define MIH_C_BIT_RDF_SCHEMA                        MIH_C_BIT_3_VALUE
#define MIH_C_BIT_IE_NETWORK_TYPE                   MIH_C_BIT_4_VALUE
#define MIH_C_BIT_IE_OPERATOR_ID                    MIH_C_BIT_5_VALUE
#define MIH_C_BIT_IE_SERVICE_PROVIDER_ID            MIH_C_BIT_6_VALUE
#define MIH_C_BIT_IE_COUNTRY_CODE                   MIH_C_BIT_7_VALUE
#define MIH_C_BIT_IE_NETWORK_ID                     MIH_C_BIT_8_VALUE
#define MIH_C_BIT_IE_NETWORK_AUX_ID                 MIH_C_BIT_9_VALUE
#define MIH_C_BIT_IE_ROAMING_PARTNERS               MIH_C_BIT_10_VALUE
#define MIH_C_BIT_IE_COST                           MIH_C_BIT_11_VALUE
#define MIH_C_BIT_IE_NETWORK_QOS                    MIH_C_BIT_12_VALUE
#define MIH_C_BIT_IE_NETWORK_DATA_RATE              MIH_C_BIT_13_VALUE
#define MIH_C_BIT_IE_NET_REGULT_DOMAIN              MIH_C_BIT_14_VALUE
#define MIH_C_BIT_IE_NET_FREQUENCY_BANDS            MIH_C_BIT_15_VALUE
#define MIH_C_BIT_IE_NET_IP_CFG_METHODS             MIH_C_BIT_16_VALUE
#define MIH_C_BIT_IE_NET_CAPABILITIES               MIH_C_BIT_17_VALUE
#define MIH_C_BIT_IE_NET_SUPPORTED_LCP              MIH_C_BIT_18_VALUE
#define MIH_C_BIT_IE_NET_MOB_MGMT_PROT              MIH_C_BIT_19_VALUE
#define MIH_C_BIT_IE_NET_EMSERV_PROXY               MIH_C_BIT_20_VALUE
#define MIH_C_BIT_IE_NET_IMS_PROXY_CSCF             MIH_C_BIT_21_VALUE
#define MIH_C_BIT_IE_NET_MOBILE_NETWORK             MIH_C_BIT_22_VALUE
#define MIH_C_BIT_IE_POA_LINK_ADDR                  MIH_C_BIT_23_VALUE
#define MIH_C_BIT_IE_POA_LOCATION                   MIH_C_BIT_24_VALUE
#define MIH_C_BIT_IE_POA_CHANNEL_RANGE              MIH_C_BIT_25_VALUE
#define MIH_C_BIT_IE_POA_SYSTEM_INFO                MIH_C_BIT_26_VALUE
#define MIH_C_BIT_IE_POA_SUBNET_INFO                MIH_C_BIT_27_VALUE
#define MIH_C_BIT_IE_POA_IP_ADDR                    MIH_C_BIT_28_VALUE
/*! \var MIH_C_MIH_IQ_TYPE_LST_T
* \brief A list of IS query types.
*        Bitmap Values:
*        Bit 0: Binary data
*        Bit 1: RDF data
*        Bit 2: RDF schema URL
*        Bit 3: RDF schema
*        Bit 4: IE_NETWORK_TYPE
*        Bit 5: IE_OPERATOR_ID
*        Bit 6: IE_SERVICE_PROVIDER_ID
*        Bit 7: IE_COUNTRY_CODE
*        Bit 8: IE_NETWORK_ID
*        Bit 9: IE_NETWORK_AUX_ID
*        Bit 10: IE_ROAMING_PARTNERS
*        Bit 11: IE_COST
*        Bit 12: IE_NETWORK_QOS
*        Bit 13: IE_NETWORK_DATA_RATE
*        Bit 14: IE_NET_REGULT_DOMAIN
*        Bit 15: IE_NET_FREQUENCY_BANDS
*        Bit 16: IE_NET_IP_CFG_METHODS
*        Bit 17: IE_NET_CAPABILITIES
*        Bit 18: IE_NET_SUPPORTED_LCP
*        Bit 19: IE_NET_MOB_MGMT_PROT
*        Bit 20: IE_NET_EMSERV_PROXY
*        Bit 21: IE_NET_IMS_PROXY_CSCF
*        Bit 22: IE_NET_MOBILE_NETWORK
*        Bit 23: IE_POA_LINK_ADDR
*        Bit 24: IE_POA_LOCATION
*        Bit 25: IE_POA_CHANNEL_RANGE
*        Bit 26: IE_POA_SYSTEM_INFO
*        Bit 27: IE_POA_SUBNET_INFO
*        Bit 28: IE_POA_IP_ADDR
*        Bit 29–63: (Reserved)
*/
TYPEDEF_BITMAP64(MIH_C_MIH_IQ_TYPE_LST)
//-------------------------------------------
#define MIH_C_BIT_UDP                        MIH_C_BIT_0_VALUE
#define MIH_C_BIT_TCP                        MIH_C_BIT_1_VALUE
/*! \var MIH_C_MIH_TRANS_LST_T
* \brief A list of supported transports.
*        Bitmap Values:
*        Bit 0: UDP
*        Bit 1: TCP
*        Bit 2–15: (Reserved)
*/
TYPEDEF_BITMAP16(MIH_C_MIH_TRANS_LST)
//-------------------------------------------
/*! \struct  MIH_C_NET_TYPE_ADDR_T
* \brief Represent a link address of a specific network type.
*/
typedef struct MIH_C_NET_TYPE_ADDR {
    MIH_C_NETWORK_TYPE_T          network_type;
    MIH_C_LINK_ADDR_T             link_addr;
} MIH_C_NET_TYPE_ADDR_T;
//-------------------------------------------
/** @}*/


//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.22—Data types for handover operation
//-----------------------------------------------------------------------------
/** \defgroup MIH_C_F22_DATA_TYPE_FOR_HANDOVER_OPERATION Data type for handover operation
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */
//TBD ASGN_RES_SET
/*! \var MIH_C_HO_CAUSE_T
* \brief Represents the reason for performing a handover.
*        Same enumeration list as link down reason code.
*        See Table F.7
*/
TYPEDEF_UNSIGNED_INT1(MIH_C_HO_CAUSE)
//-------------------------------------------
/*! \var MIH_C_HO_RESULT_T
* \brief Handover result.
*        0: Success
*        1: Failure
*        2: Rejected
*/
TYPEDEF_ENUMERATED(MIH_C_HO_RESULT)
#define MIH_C_HO_RESULT_SUCCESS            (MIH_C_HO_RESULT_T)0
#define MIH_C_HO_RESULT_FAILURE            (MIH_C_HO_RESULT_T)1
#define MIH_C_HO_RESULT_REJECTED           (MIH_C_HO_RESULT_T)2
//-------------------------------------------
/*! \var MIH_C_HO_STATUS_T
* \brief Represents the permission for handover.
*        0: HandoverPermitted
*        1: HandoverDeclined
*/
TYPEDEF_ENUMERATED(MIH_C_HO_STATUS)
#define MIH_C_HO_STATUS_HANDOVER_PERMITTED       (MIH_C_HO_STATUS_T)0
#define MIH_C_HO_STATUS_HANDOVER_DECLINED        (MIH_C_HO_STATUS_T)1
//-------------------------------------------
/*! \var MIH_C_PREDEF_CFG_ID_T
* \brief Pre-defined configuration identifier.
*        0..255
*/
TYPEDEF_INTEGER1(MIH_C_PREDEF_CFG_ID)
//-------------------------------------------
//TBD RQ_RESULT
//TBD REQ_RES_SET
//TBD TGT_NET_INFO
//TBD TSP_CARRIER
//TBD TSP_CONTAINER
/** @}*/


//-----------------------------------------------------------------------------
// STD 802.21-2008 Table F.23—Data type for MIH_NET_SAP primitives
//-----------------------------------------------------------------------------
/** \defgroup MIH_C_F23_DATA_TYPE_FOR_MIH_NET_SAP_PRIMITIVES Data type for MIH NET SAP primitives.
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */
/*! \var MIH_C_TRANSPORT_TYPE_T
* \brief The transport type supported:
*        0: L2
*        1: L3 or higher layer protocols
*/
TYPEDEF_ENUMERATED(MIH_C_TRANSPORT_TYPE)
#define MIH_C_TRANSPORT_TYPE_L2                      (MIH_C_TRANSPORT_TYPE_T)0
#define MIH_C_TRANSPORT_TYPE_L3_OR_HIGHER_PROTOCOLS  (MIH_C_TRANSPORT_TYPE_T)1

/** @}*/




/**
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */

#define MIH_C_TLV_SOURCE_MIHF_ID                      (MIH_C_INTEGER1_T)1
#define MIH_C_TLV_DESTINATION_MIHF_ID                 (MIH_C_INTEGER1_T)2
#define MIH_C_TLV_STATUS                              (MIH_C_INTEGER1_T)3
#define MIH_C_TLV_LINK_TYPE                           (MIH_C_INTEGER1_T)4
#define MIH_C_TLV_MIH_EVENT_LIST                      (MIH_C_INTEGER1_T)5
#define MIH_C_TLV_MIH_COMMAND_LIST                    (MIH_C_INTEGER1_T)6
#define MIH_C_TLV_MIIS_QUERY_TYPE_LIST                (MIH_C_INTEGER1_T)7
#define MIH_C_TLV_TRANSPORT_OPTION_LIST               (MIH_C_INTEGER1_T)8
#define MIH_C_TLV_LINK_ADDRESS_LIST                   (MIH_C_INTEGER1_T)9
#define MIH_C_TLV_MBB_HANDOVER_SUPPORT                (MIH_C_INTEGER1_T)10
#define MIH_C_TLV_REGISTER_REQUEST_CODE               (MIH_C_INTEGER1_T)11
#define MIH_C_TLV_VALID_TIME_INTERVAL                 (MIH_C_INTEGER1_T)12
#define MIH_C_TLV_LINK_IDENTIFIER                     (MIH_C_INTEGER1_T)13
#define MIH_C_TLV_NEW_LINK_IDENTIFIER                 (MIH_C_INTEGER1_T)14
#define MIH_C_TLV_OLD_ACCESS_ROUTER                   (MIH_C_INTEGER1_T)15
#define MIH_C_TLV_NEW_ACCESS_ROUTER                   (MIH_C_INTEGER1_T)16
#define MIH_C_TLV_IP_RENEWAL_FLAG                     (MIH_C_INTEGER1_T)17
#define MIH_C_TLV_MOBILITY_MANAGEMENT_SUPPORT         (MIH_C_INTEGER1_T)18
#define MIH_C_TLV_IP_ADDRESS_CONFIGURATION_METHODS    (MIH_C_INTEGER1_T)19
#define MIH_C_TLV_LINK_DOWN_REASON_CODE               (MIH_C_INTEGER1_T)20
#define MIH_C_TLV_LINK_TIME_INTERVAL                  (MIH_C_INTEGER1_T)21
#define MIH_C_TLV_LINK_GOING_DOWN_REASON_CODE         (MIH_C_INTEGER1_T)22
#define MIH_C_TLV_LINK_PARAMETER_REPORT_LIST          (MIH_C_INTEGER1_T)23
#define MIH_C_TLV_DEVICE_STATES_REQUEST               (MIH_C_INTEGER1_T)24
#define MIH_C_TLV_LINK_IDENTIFIER_LIST                (MIH_C_INTEGER1_T)25
#define MIH_C_TLV_DEVICE_STATES_RESPONSE_LIST         (MIH_C_INTEGER1_T)26
#define MIH_C_TLV_GET_STATUS_REQUEST_SET              (MIH_C_INTEGER1_T)27
#define MIH_C_TLV_GET_STATUS_RESPONSE_LIST            (MIH_C_INTEGER1_T)28
#define MIH_C_TLV_CONFIGURE_REQUEST_LIST              (MIH_C_INTEGER1_T)29
#define MIH_C_TLV_CONFIGURE_RESPONSE_LIST             (MIH_C_INTEGER1_T)30
#define MIH_C_TLV_LIST_OF_LINK_POA_LIST               (MIH_C_INTEGER1_T)31
#define MIH_C_TLV_PREFERRED_LINK_LIST                 (MIH_C_INTEGER1_T)32
#define MIH_C_TLV_HANDOVER_RESOURCE_QUERY_LIST        (MIH_C_INTEGER1_T)33
#define MIH_C_TLV_HANDOVER_STATUS                     (MIH_C_INTEGER1_T)34
#define MIH_C_TLV_ACCESS_ROUTER_ADDRESS               (MIH_C_INTEGER1_T)35
#define MIH_C_TLV_DHCP_SERVER_ADDRESS                 (MIH_C_INTEGER1_T)36
#define MIH_C_TLV_FA_ADDRESS                          (MIH_C_INTEGER1_T)37
#define MIH_C_TLV_LINK_ACTIONS_LIST                   (MIH_C_INTEGER1_T)38
#define MIH_C_TLV_LINK_ACTIONS_RESULT_LIST            (MIH_C_INTEGER1_T)39
#define MIH_C_TLV_HANDOVER_RESULT                     (MIH_C_INTEGER1_T)40
#define MIH_C_TLV_RESOURCE_STATUS                     (MIH_C_INTEGER1_T)41
#define MIH_C_TLV_RESOURCE_RETENTION_STATUS           (MIH_C_INTEGER1_T)42
#define MIH_C_TLV_INFO_QUERY_BINARY_DATA_LIST         (MIH_C_INTEGER1_T)43
#define MIH_C_TLV_INFO_QUERY_RDF_DATA_LIST            (MIH_C_INTEGER1_T)44
#define MIH_C_TLV_INFO_QUERY_RDF_SCHEMA_URL           (MIH_C_INTEGER1_T)45
#define MIH_C_TLV_INFO_QUERY_RDF_SCHEMA_LIST          (MIH_C_INTEGER1_T)46
#define MIH_C_TLV_MAX_RESPONSE_SIZE                   (MIH_C_INTEGER1_T)47
#define MIH_C_TLV_INFO_RESPONSE_BINARY_DATA_LIST      (MIH_C_INTEGER1_T)48
#define MIH_C_TLV_INFO_RESPONSE_RDF_DATA_LIST         (MIH_C_INTEGER1_T)49
#define MIH_C_TLV_INFO_RESPONSE_RDF_SCHEMA_URL_LIST   (MIH_C_INTEGER1_T)50
#define MIH_C_TLV_INFO_RESPONSE_RDF_SCHEMA_LIST       (MIH_C_INTEGER1_T)51
#define MIH_C_TLV_MOBILE_NODE_MIHF_ID                 (MIH_C_INTEGER1_T)52
#define MIH_C_TLV_QUERY_RESOURCE_REPORT_FLAG          (MIH_C_INTEGER1_T)53
#define MIH_C_TLV_EVENT_CONFIGURATION_INFO_LIST       (MIH_C_INTEGER1_T)54
#define MIH_C_TLV_TARGET_NETWORK_INFO                 (MIH_C_INTEGER1_T)55
#define MIH_C_TLV_LIST_OF_TARGET_NETWORK_INFO         (MIH_C_INTEGER1_T)56
#define MIH_C_TLV_ASSIGNED_RESOURCE_SET               (MIH_C_INTEGER1_T)57
#define MIH_C_TLV_LINK_DETECTED_INFO_LIST             (MIH_C_INTEGER1_T)58
#define MIH_C_TLV_MN_LINK_ID                          (MIH_C_INTEGER1_T)59
#define MIH_C_TLV_POA                                 (MIH_C_INTEGER1_T)60
#define MIH_C_TLV_UNAUTHENTICATED_INFORMATION_REQUEST (MIH_C_INTEGER1_T)61
#define MIH_C_TLV_NETWORK_TYPE                        (MIH_C_INTEGER1_T)62
#define MIH_C_TLV_REQUESTED_RESOURCE_SET              (MIH_C_INTEGER1_T)63

#ifdef MIH_C_MEDIEVAL_EXTENSIONS
#    define MIH_C_TLV_LINK_EVENT_LIST                     (MIH_C_INTEGER1_T)101
#    define MIH_C_TLV_LINK_CMD_LIST                       (MIH_C_INTEGER1_T)102
#    define MIH_C_TLV_LINK_PARAM_TYPE_LIST                (MIH_C_INTEGER1_T)103
#    define MIH_C_TLV_LINK_PARAMETERS_STATUS_LIST         (MIH_C_INTEGER1_T)104
#    define MIH_C_TLV_LINK_STATES_REQ                     (MIH_C_INTEGER1_T)105
#    define MIH_C_TLV_LINK_STATES_RSP_LIST                (MIH_C_INTEGER1_T)106
#    define MIH_C_TLV_LINK_DESC_REQ                       (MIH_C_INTEGER1_T)107
#    define MIH_C_TLV_LINK_DESC_RSP_LIST                  (MIH_C_INTEGER1_T)108
#    define MIH_C_TLV_LINK_ACTION                         (MIH_C_INTEGER1_T)109
#    define MIH_C_TLV_LINK_AC_RESULT                      (MIH_C_INTEGER1_T)110
#    define MIH_C_TLV_LINK_SCAN_RSP_LIST                  (MIH_C_INTEGER1_T)111
#    define MIH_C_TLV_LINK_DET_INFO                       (MIH_C_INTEGER1_T)112
#    define MIH_C_TLV_LINK_INTERFACE_TYPE_ADDR            (MIH_C_INTEGER1_T)113
#    define MIH_C_TLV_MOS_DSCV                            (MIH_C_INTEGER1_T)114
#endif
/** @}*/

//-----------------------------------------------------------------------------
#endif
