/*********************************************************************
                          rrc_qos_dfinitions.h  -  description
                             -------------------
    copyright            : (C) 2001, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 *********************************************************************
  QoS classes supported in the system
 ********************************************************************/
#ifndef __RRC_QOS_DEFS_CELL_H__
#define __RRC_QOS_DEFS_CELL_H__
#include "rrc_qos_classes.h"


int rrc_qos_classes_definition [RRC_QOS_LAST][5] = {
     //{traffic_class, guaranted_bit_rate_uplink, max_bit_rate_uplink, guaranted_bit_rate_downlink, max_bit_rate_downlink},
      // 1 -RRC_QOS_CONV_32_32:
     {CONVERSATIONAL,32,32,32,32},
      // 2 -RRC_QOS_CONV_64_64:
     {CONVERSATIONAL,64,64,64,64},
      // 3 - RRC_QOS_CONV_128_128:
     {CONVERSATIONAL,128,128,128,128},
      // 4 -RRC_QOS_CONV_256_256:
     {CONVERSATIONAL,256,256,256,256},
      // 5 -RRC_QOS_CONV_320_320:
     {CONVERSATIONAL,320,320,320,320},
      // 6 -RRC_QOS_INTER_64_32:
     {INTERACTIVE,32,32,64,64},
      // 7 -RRC_QOS_INTER_128_32:
     {INTERACTIVE,32,32,128,128},
      // 8 -RRC_QOS_INTER_256_32:
     {INTERACTIVE,32,32,256,256},
      // 9 -RRC_QOS_INTER_384_32:
     {INTERACTIVE,32,32,384,384},
      //10 -RRC_QOS_INTER_64_64:
     {INTERACTIVE,64,64,64,64},
      //11 -RRC_QOS_INTER_128_64:
     {INTERACTIVE,64,64,128,128},
      //12 -RRC_QOS_INTER_256_64:
     {INTERACTIVE,64,64,256,256},
      //13 -RRC_QOS_INTER_384_64:
     {INTERACTIVE,64,64,384,384},
      //14 -RRC_QOS_INTER_448_64:
     {INTERACTIVE,64,64,448,448},
      //15 -RRC_QOS_INTER_128_128:
     {INTERACTIVE,128,128,128,128},
      //16 -RRC_QOS_INTER_256_128:
     {INTERACTIVE,128,128,256,256},
      //17 -RRC_QOS_INTER_384_128:
     {INTERACTIVE,128,128,384,384},
      //18 -RRC_QOS_INTER_256_256:
     {INTERACTIVE,256,256,256,256},
      //19 -RRC_QOS_INTER_320_320:
     {INTERACTIVE,320,320,320,320},
      //20 -RRC_QOS_MBMS_STREAMING_128:
     {STREAMING,0,0,128,128},
      //21 -RRC_QOS_MBMS_STREAMING_256:
     {STREAMING,0,0,256,256},
      //22 -RRC_QOS_MBMS_STREAMING_384:
     {STREAMING,0,0,384,384},
      //23 -RRC_QOS_STREAMING_896_256:
     {STREAMING,256,256,896,896}
};

char *rrc_traffic_class_names[4] = {"CONVERSATIONAL","STREAMING","INTERACTIVE","BACKGROUND"};


#endif

