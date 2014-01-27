/*****************************************************************************
			Eurecom OpenAirInterface 3
			Copyright(c) 2012 Eurecom

Source		usim_api.h

Version		0.1

Date		2012/10/09

Product		NAS stack

Subsystem	Application Programming Interface

Author		Frederic Maurel

Description	Implements the API used by the NAS layer to read/write
		data to/from the USIM application

*****************************************************************************/

#ifdef NAS_UE

#include "usim_api.h"
#include "nas_log.h"
#include "memory.h"

#include "aka_functions.h"
#include <string.h>	// memcpy, memset
#include <stdlib.h>	// malloc, free

/****************************************************************************/
/****************  E X T E R N A L    D E F I N I T I O N S  ****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/*
 * The name of the file where are stored data of the USIM application
 */
#define USIM_API_NVRAM_FILENAME	".usim.nvram"

/*
 * The name of the environment variable which defines the directory
 * where the USIM application file is located
 */
#define USIM_API_NVRAM_DIRNAME	"USIM_DIR"

/*
 * Subscriber authentication security key
 */
#define USIM_API_K_SIZE         16
#define USIM_API_K_VALUE        "8BAF473F2F8FD09487CCCBD7097C6862"

static UInt8_t _usim_api_k[USIM_API_K_SIZE];


/*
 * List of last used Sequence Numbers SQN
 */
static struct _usim_api_data_s {
	/* Highest sequence number the USIM has ever accepted	*/
    UInt32_t sqn_ms;
	/* List of the last used sequence numbers		*/
#define USIM_API_SQN_LIST_SIZE	32
    UInt8_t n_sqns;
    UInt32_t sqn[USIM_API_SQN_LIST_SIZE];
} _usim_api_data;

static UInt8_t _usim_api_hex_char_to_hex_value (char c);
static void _usim_api_hex_string_to_hex_value (UInt8_t *hex_value, const char *hex_string, int size);
static int _usim_api_check_sqn(UInt32_t seq, UInt8_t ind);

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:        usim_api_read()                                           **
 **                                                                        **
 ** Description: Reads data from the USIM application                      **
 **                                                                        **
 ** Inputs:      None                                                      **
 **              Others:        File where are stored USIM data            **
 **                                                                        **
 ** Outputs:     data:          Pointer to the USIM application data       **
 **              Return:        RETURNerror, RETURNok                      **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
int usim_api_read(usim_data_t* data)
{
    LOG_FUNC_IN;

    /* Get USIM application pathname */
    char* path = memory_get_path(USIM_API_NVRAM_DIRNAME,
				 USIM_API_NVRAM_FILENAME);
    if (path == NULL) {
	LOG_TRACE(ERROR, "USIM-API  - Failed to get USIM pathname");
	LOG_FUNC_RETURN (RETURNerror);
    }

    /* Read USIM application data */
    if (memory_read(path, data, sizeof(usim_data_t)) != RETURNok) {
	LOG_TRACE(ERROR, "USIM-API  - %s file is either not valid "
		  "or not present", path);
	free(path);
	LOG_FUNC_RETURN (RETURNerror);
    }

    /* initialize the subscriber authentication security key */
    _usim_api_hex_string_to_hex_value(_usim_api_k, USIM_API_K_VALUE, USIM_API_K_SIZE);

    free(path);
    LOG_FUNC_RETURN (RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:        usim_api_write()                                          **
 **                                                                        **
 ** Description: Writes data to the USIM application                       **
 **                                                                        **
 ** Inputs:      data:          Pointer to the USIM application data       **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **              Return:        RETURNerror, RETURNok                      **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
int usim_api_write(const usim_data_t* data)
{
    LOG_FUNC_IN;

    /* Get USIM application pathname */
    char* path = memory_get_path(USIM_API_NVRAM_DIRNAME,
				 USIM_API_NVRAM_FILENAME);
    if (path == NULL) {
	LOG_TRACE(ERROR, "USIM-API  - Failed to get USIM pathname");
	LOG_FUNC_RETURN (RETURNerror);
    }

    /* Write USIM application data */
    if (memory_write(path, data, sizeof(usim_data_t)) != RETURNok) {

	LOG_TRACE(ERROR, "USIM-API  - Unable to write USIM file %s", path);
	free(path);
	LOG_FUNC_RETURN (RETURNerror);
    }

    free(path);
    LOG_FUNC_RETURN (RETURNok);
}

/****************************************************************************
 **                                                                        **
 ** Name:        usim_api_authenticate()                                   **
 **                                                                        **
 ** Description: Performs mutual authentication of the USIM to the network,**
 **              checking whether authentication token AUTN can be accep-  **
 **              ted. If so, returns an authentication response RES and    **
 **              the ciphering and integrity keys.                         **
 **              In case of synch failure, returns a re-synchronization    **
 **              token AUTS.                                               **
 **                                                                        **
 **              3GPP TS 31.102, section 7.1.1.1                           **
 **                                                                        **
 **              Authentication and key generating function algorithms are **
 **              specified in 3GPP TS 35.206.                              **
 **                                                                        **
 ** Inputs:      rand:          Random challenge number                    **
 **              autn:          Authentication token                       **
 **                             AUTN = (SQN xor AK) || AMF || MAC          **
 **                                         48          16     64 bits     **
 **              Others:        Security key                               **
 **                                                                        **
 ** Outputs:     auts:          Re-synchronization token                   **
 **              res:           Authentication response                    **
 **              ck:            Ciphering key                              **
 **              ik             Integrity key                              **
 **                                                                        **
 **              Return:        RETURNerror, RETURNok                      **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
int usim_api_authenticate(const OctetString* rand, const OctetString* autn,
                          OctetString* auts, OctetString* res,
                          OctetString* ck, OctetString* ik)
{
    LOG_FUNC_IN;

    int rc;
    int i;

    /* Compute the authentication response RES = f2K (RAND) */
    /* Compute the cipher key CK = f3K (RAND) */
    /* Compute the integrity key IK = f4K (RAND) */
    /* Compute the anonymity key AK = f5K (RAND) */
#define USIM_API_AK_SIZE 6
    u8 ak[USIM_API_AK_SIZE];
    f2345(_usim_api_k, rand->value,
          res->value, ck->value, ik->value, ak);

    /* Retrieve the sequence number SQN = (SQN ⊕ AK) ⊕ AK */
#define USIM_API_SQN_SIZE USIM_API_AK_SIZE
    u8 sqn[USIM_API_SQN_SIZE];
    for (i = 0; i < USIM_API_SQN_SIZE; i++) {
        sqn[i] = rand->value[i] ^ ak[i];
    }

    /* Compute XMAC = f1K (SQN || RAND || AMF) */
#define USIM_API_XMAC_SIZE 8
    u8 xmac[USIM_API_XMAC_SIZE];
    f1(_usim_api_k, rand->value, sqn, &rand->value[USIM_API_SQN_SIZE], xmac);

    /* Compare the XMAC with the MAC included in AUTN */
#if 0 // TODO !!! TO BE REMOVED
#define USIM_API_AMF_SIZE 2
    if ( memcmp(xmac, &rand->value[USIM_API_SQN_SIZE + USIM_API_AMF_SIZE],
            USIM_API_XMAC_SIZE) != 0 ) {
        LOG_FUNC_RETURN (RETURNerror);
    }
#endif // TODO !!! TO BE REMOVED

    /* Verify that the received sequence number SQN is in the correct range */
    rc = _usim_api_check_sqn(*(UInt32_t*)(sqn), sqn[USIM_API_SQN_SIZE - 1]);
    if (rc != RETURNok) {
        /* Synchronisation failure; compute the AUTS parameter */

        /* Concealed value of the counter SQNms in the USIM:
         * Conc(SQNMS) = SQNMS ⊕ f5*K(RAND) */
        f5star(_usim_api_k, rand->value, ak);

#define USIM_API_SQNMS_SIZE USIM_API_SQN_SIZE
        u8 sqn_ms[USIM_API_SQNMS_SIZE];
        memset(sqn_ms, 0, USIM_API_SQNMS_SIZE);
#define USIM_API_SQN_MS_SIZE	3
        for (i = 0; i < USIM_API_SQN_MS_SIZE; i++) {
            sqn_ms[USIM_API_SQNMS_SIZE - i] =
            ((UInt8_t*)(_usim_api_data.sqn_ms))[USIM_API_SQN_MS_SIZE - i];
        }

        u8 sqnms[USIM_API_SQNMS_SIZE];
        for (i = 0; i < USIM_API_SQNMS_SIZE; i++) {
            sqnms[i] = sqn_ms[i] ^ ak[i];
        }

        /* Synchronisation message authentication code:
         * MACS = f1*K(SQNMS || RAND || AMF) */
#define USIM_API_MACS_SIZE USIM_API_XMAC_SIZE
        u8 macs[USIM_API_MACS_SIZE];
        f1star(_usim_api_k, rand->value, sqn_ms,
               &rand->value[USIM_API_SQN_SIZE], macs);

        /* Synchronisation authentication token:
         * AUTS = Conc(SQNMS) || MACS */
        memcpy(&auts->value[0], sqnms, USIM_API_SQNMS_SIZE);
        memcpy(&auts->value[USIM_API_SQNMS_SIZE], macs, USIM_API_MACS_SIZE);
    }

    LOG_FUNC_RETURN (RETURNok);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:        _usim_api_hex_char_to_hex_value()                         **
 **                                                                        **
 ** Description: Converts an hexadecimal ASCII coded digit into its value. **
 **                                                                        **
 ** Inputs:      c:             A char holding the ASCII coded value       **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **              Return:        Converted value                            **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
static UInt8_t _usim_api_hex_char_to_hex_value (char c)
{
    if (c >= 'A')
    {
        /* Remove case bit */
        c &= ~('a' ^ 'A');

        return (c - 'A' + 10);
    }
    else
    {
        return (c - '0');
    }
}

/****************************************************************************
 **                                                                        **
 ** Name:        _usim_api_hex_string_to_hex_value()                       **
 **                                                                        **
 ** Description: Converts an hexadecimal ASCII coded string into its value.**
 **                                                                        **
 ** Inputs:      hex_value:     A pointer to the location to store the     **
 **                             conversion result                          **
 **              size:          The size of hex_value in bytes             **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     hex_value:     Converted value                            **
 **              Return:        None                                       **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
static void _usim_api_hex_string_to_hex_value (UInt8_t *hex_value, const char *hex_string, int size)
{
    int i;

    for (i=0; i < size; i++)
    {
        hex_value[i] = (_usim_api_hex_char_to_hex_value(hex_string[2 * i]) << 4) | _usim_api_hex_char_to_hex_value(hex_string[2 * i + 1]);
    }
}

/****************************************************************************
 **                                                                        **
 ** Name:        _usim_api_check_sqn()                                     **
 **                                                                        **
 ** Description: Verifies the freshness of sequence numbers to determine   **
 **              whether the specified sequence number is in the correct   **
 **              range and acceptabled by the USIM.                        **
 **                                                                        **
 **              3GPP TS 31.102, Annex C.2                                 **
 **                                                                        **
 ** Inputs:      seq:           Sequence number value                      **
 **              ind:           Index value                                **
 **              Others:        None                                       **
 **                                                                        **
 ** Outputs:     None                                                      **
 **              Return:        RETURNerror, RETURNok                      **
 **              Others:        None                                       **
 **                                                                        **
 ***************************************************************************/
static int _usim_api_check_sqn(UInt32_t seq, UInt8_t ind)
{
    /* TODO */
    return (RETURNok);
}

#endif // NAS_UE
