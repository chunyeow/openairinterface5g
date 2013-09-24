/*******************************************************************************
 * 
 * Eurecom OpenAirInterface 1
 * Copyright(c) 1999 - 2012 Eurecom
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Contact Information
 * Openair Admin: openair_admin@eurecom.fr
 * Openair Tech : openair_tech@eurecom.fr
 * Forums       : http://forums.eurecom.fsr/openairinterface
 * Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France
 *
 *******************************************************************************/

/*! \file vcd_signal_dumper.h
 * \brief Output functions call to VCD file which is readable by gtkwave.
 * \author ROUX Sebastien
 * \date 2012
 * \version 0.1
 * \note
 * \bug
 * \warning
 */

#ifndef VCD_SIGNAL_DUMPER_H_
#define VCD_SIGNAL_DUMPER_H_

//#define ENABLE_USE_CPU_EXECUTION_TIME

/* WARNING: if you edit the enums below, update also string definitions in vcd_signal_dumper.c */
typedef enum
{
    VCD_SIGNAL_DUMPER_VARIABLES_FRAME_NUMBER = 0,
    VCD_SIGNAL_DUMPER_VARIABLES_SLOT_NUMBER,
    VCD_SIGNAL_DUMPER_VARIABLES_DAQ_MBOX,
    VCD_SIGNAL_DUMPER_VARIABLES_DIFF,
    VCD_SIGNAL_DUMPER_VARIABLES_LAST,
    VCD_SIGNAL_DUMPER_VARIABLES_END = VCD_SIGNAL_DUMPER_VARIABLES_LAST,
} vcd_signal_dump_variables;

typedef enum
{
    VCD_SIGNAL_DUMPER_FUNCTIONS_MACPHY_INIT = 0,
    VCD_SIGNAL_DUMPER_FUNCTIONS_MACPHY_EXIT,
    VCD_SIGNAL_DUMPER_FUNCTIONS_ENB_DLSCH_ULSCH_SCHEDULER,
    VCD_SIGNAL_DUMPER_FUNCTIONS_FILL_RAR,
    VCD_SIGNAL_DUMPER_FUNCTIONS_TERMINATE_RA_PROC,
    VCD_SIGNAL_DUMPER_FUNCTIONS_INITIATE_RA_PROC,
    VCD_SIGNAL_DUMPER_FUNCTIONS_CANCEL_RA_PROC,
    VCD_SIGNAL_DUMPER_FUNCTIONS_GET_DCI_SDU,
    VCD_SIGNAL_DUMPER_FUNCTIONS_GET_DLSCH_SDU,
    VCD_SIGNAL_DUMPER_FUNCTIONS_RX_SDU,
    VCD_SIGNAL_DUMPER_FUNCTIONS_MRBCH_PHY_SYNC_FAILURE,
    VCD_SIGNAL_DUMPER_FUNCTIONS_SR_INDICATION,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_CONFIG_SIB1_ENB,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_CONFIG_SIB2_ENB,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_CONFIG_DEDICATED_ENB,
    VCD_SIGNAL_DUMPER_FUNCTIONS_OUT_OF_SYNC_IND,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_SI,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_CCCH,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_DECODE_BCCH,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_SEND_SDU,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_GET_SDU,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_GET_RACH,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_PROCESS_RAR,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_SCHEDULER,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_GET_SR,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_MEASUREMENT_PROCEDURES,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_PDCCH_PROCEDURES,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_PBCH_PROCEDURES,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_PROCEDURES_ENB_TX,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_PROCEDURES_ENB_RX,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_PROCEDURES_UE_TX,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_PROCEDURES_UE_RX,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_PROCEDURES_ENB_LTE,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_PROCEDURES_UE_LTE,
    VCD_SIGNAL_DUMPER_FUNCTIONS_EMU_TRANSPORT,
    VCD_SIGNAL_DUMPER_FUNCTIONS_LOG_RECORD,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_RUN,
    VCD_SIGNAL_DUMPER_FUNCTIONS_RT_SLEEP,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PDSCH_THREAD,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_THREAD0,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_THREAD1,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_THREAD2,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_THREAD3,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_THREAD4,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_THREAD5,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_THREAD6,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_THREAD7,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_DECODING0,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_DECODING1,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_DECODING2,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_DECODING3,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_DECODING4,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_DECODING5,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_DECODING6,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_DECODING7,
    VCD_SIGNAL_DUMPER_FUNCTIONS_RX_PDCCH,
    VCD_SIGNAL_DUMPER_FUNCTIONS_DCI_DECODING,
    VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_MAC_CONFIG,
    VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_UE_DECODE_SIB1,
    VCD_SIGNAL_DUMPER_FUNCTIONS_RRC_UE_DECODE_SI,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_UE_CONFIG_SIB2,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_UE_COMPUTE_PRACH,    
    VCD_SIGNAL_DUMPER_FUNCTIONS_PHY_ENB_ULSCH_DECODING, 
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_ULSCH_MODULATION,
    VCD_SIGNAL_DUMPER_FUNCTIONS_UE_ULSCH_ENCODING,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_APPLY_SECURITY,
    VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_VALIDATE_SECURITY,
    VCD_SIGNAL_DUMPER_FUNCTIONS_TEST,
    VCD_SIGNAL_DUMPER_FUNCTIONS_LAST,
    VCD_SIGNAL_DUMPER_FUNCTIONS_END = VCD_SIGNAL_DUMPER_FUNCTIONS_LAST,
} vcd_signal_dump_functions;

typedef enum
{
    VCD_SIGNAL_DUMPER_MODULE_VARIABLES,
    VCD_SIGNAL_DUMPER_MODULE_FUNCTIONS,
//     VCD_SIGNAL_DUMPER_MODULE_UE_PROCEDURES_FUNCTIONS,
    VCD_SIGNAL_DUMPER_MODULE_LAST,
    VCD_SIGNAL_DUMPER_MODULE_END = VCD_SIGNAL_DUMPER_MODULE_LAST,
} vcd_signal_dumper_modules;

typedef enum
{
    VCD_FUNCTION_OUT,
    VCD_FUNCTION_IN,
    VCD_FUNCTION_LAST,
} vcd_signal_dump_in_out;

typedef enum {
    VCD_REAL, // REAL = variable
    VCD_WIRE, // WIRE = Function
} vcd_signal_type;

/*!
 * \brief Init function for the vcd dumper.
 * @param None
 */
void vcd_signal_dumper_init(char* filename);
/*!
 * \brief Close file descriptor.
 * @param None
 */
void vcd_signal_dumper_close(void);
/*!
 * \brief Create header for VCD file.
 * @param None
 */
void vcd_signal_dumper_create_header(void);
/*!
 * \brief Dump state of a variable
 * @param Name of the variable to dump (see the corresponding enum)
 * @param Value of the variable to dump (type: unsigned long)
 */
void vcd_signal_dumper_dump_variable_by_name(vcd_signal_dump_variables variable_name,
                                             unsigned long             value);
/*!
 * \brief Dump function usage
 * @param Name Function name to dump (see the corresponding enum)
 * @param State: either VCD_FUNCTION_START or VCD_FUNCTION_END
 */
void vcd_signal_dumper_dump_function_by_name(vcd_signal_dump_functions  function_name,
                                             vcd_signal_dump_in_out     function_in_out);

extern int ouput_vcd;

#endif /* !defined (VCD_SIGNAL_DUMPER_H_) */

