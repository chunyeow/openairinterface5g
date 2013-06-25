/***************************************************************************
                          	pdcp_control_primitives_proto_extern.h
                             -------------------
    begin                : Mon Dec 10 2001
    copyright            : (C) 2001 by EURECOM
    email                : Lionel.Gauthier@eurecom.fr
                             -------------------
    description
    This file contains the prototypes of functions used for configuration of pdcp

 ***************************************************************************/
#ifndef PDCP_CONTROL_PRIMITIVES_PROTO_EXTERN_H
#    define PDCP_CONTROL_PRIMITIVES_PROTO_EXTERN_H
#    include "pdcp.h"
void            rrc_configure_pdcp (struct pdcp_entity *pdcpP, void *rlcP, u8 rlc_sap_typeP, u8 header_compression_typeP);
#endif
