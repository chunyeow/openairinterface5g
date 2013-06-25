/***************************************************************************
                          rrc_rrm_sap.h
                          -------------------
    copyright            : (C) 2010 by Eurecom
    created by           : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Definition of RRC FIFOs and SAPs to interface RRM
 **************************************************************************/
#ifndef __RRC_RRM_SAP_H__
#define __RRC_RRM_SAP_H__

#ifndef USER_MODE
#define RRM_INPUT_DEVICE     "/dev/rtf32"
#define RRM_OUTPUT_DEVICE    "/dev/rtf33"
#define RRM_INPUT_SAPI       32
#define RRM_OUTPUT_SAPI      33
#else
#define RRM_INPUT_DEVICE  "./RG_RRM_I"
#define RRM_OUTPUT_DEVICE "./RG_RRM_O"
#define RRM_INPUT_SAPI    "./RG_RRM_I"
#define RRM_OUTPUT_SAPI   "./RG_RRM_O"
#endif

#endif

