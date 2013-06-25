/***************************************************************************
                          as_configuration.h  -  description
                             -------------------
    copyright            : (C) 2003, 2010 by Eurecom
    contact              : www.eurecom.fr
 **************************************************************************
  Insert the User equipement IMEI
 ***************************************************************************/
#ifndef __AS_CONFIG_H__
#    define __AS_CONFIG_H__

#    define IMEI_D0 3
#    define IMEI_D1 5
#    define IMEI_D2 6
#    define IMEI_D3 0
#    define IMEI_D4 0
#    define IMEI_D5 0
#    define IMEI_D6 8
#    define IMEI_D7 0
#    define IMEI_D8 1
#    define IMEI_D9 4
#    define IMEI_D10 9
#    define IMEI_D11 1
#    define IMEI_D12 5
#    ifdef MOBILE0
#        define IMEI_D13 0
#    endif
#    ifdef MOBILE1
#        define IMEI_D13 1
#    endif
#endif
