#define MRAL_MODULE
#define MRALLTE_GET_C
#include "mRALlte_get.h"

//-----------------------------------------------------------------------------
void MIH_C_3GPP_ADDR_load_3gpp_str_address(MIH_C_3GPP_ADDR_T* _3gpp_addrP, u_int8_t* strP)
//-----------------------------------------------------------------------------
{
    int           i, l;
    u_int8_t      val_temp;
    unsigned char address_3gpp[32];
    unsigned char buf[3];
    u_int8_t _3gpp_byte_address[8];

    strcpy((char *)address_3gpp, (char *)strP);
    for(l=0; l<8; l++)
    {
        i=l*2;
        buf[0]= address_3gpp[i];
        buf[1]= address_3gpp[i+1];
        buf[2]= '\0';
        //sscanf((const char *)buf,"%02x", &val_temp);
        sscanf((const char *)buf,"%hhx", &val_temp);
        _3gpp_byte_address[l] = val_temp;
    }
    MIH_C_3GPP_ADDR_set(_3gpp_addrP, _3gpp_byte_address, 8);
}