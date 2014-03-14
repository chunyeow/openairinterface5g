/*******************************************************************************
Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2014 Eurecom

This program is free software; you can redistribute it and/or modify it
under the terms and conditions of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

The full GNU General Public License is included in this distribution in
the file called "COPYING".

Contact Information
Openair Admin: openair_admin@eurecom.fr
Openair Tech : openair_tech@eurecom.fr
Forums       : http://forums.eurecom.fsr/openairinterface
Address      : EURECOM,
               Campus SophiaTech,
               450 Route des Chappes,
               CS 50193
               06904 Biot Sophia Antipolis cedex,
               FRANCE
*******************************************************************************/
#    ifndef __RLC_U_TEST_H__
#        define __RLC_UM_TEST_H__
#        ifdef RLC_UM_TEST_C
#            define private_rlc_um_test(x)    x
#            define protected_rlc_um_test(x)  x
#            define public_rlc_um_test(x)     x
#        else
#            ifdef RLC_UM_MODULE
#                define private_rlc_um_test(x)
#                define protected_rlc_um_test(x)  extern x
#                define public_rlc_um_test(x)     extern x
#            else
#                define private_rlc_um_test(x)
#                define protected_rlc_um_test(x)
#                define public_rlc_um_test(x)     extern x
#            endif
#        endif
public_rlc_um_test(rlc_um_entity_t       um_tx;)
public_rlc_um_test(rlc_um_entity_t       um_rx;)

protected_rlc_um_test(void rlc_um_v9_3_0_test_windows_5(void);)
protected_rlc_um_test(void rlc_um_v9_3_0_test_windows_10(void);)
protected_rlc_um_test(void rlc_um_v9_3_0_test_data_conf(module_id_t module_idP, rb_id_t rb_idP, mui_t muiP, rlc_tx_status_t statusP);)
protected_rlc_um_test(void rlc_um_v9_3_0_test_send_sdu(rlc_um_entity_t *um_txP, int sdu_indexP);)
protected_rlc_um_test(void rlc_um_v9_3_0_test_exchange_pdus(rlc_um_entity_t *um_txP,rlc_um_entity_t *um_RxP, uint16_t bytes_txP,uint16_t bytes_rxP);)
protected_rlc_um_test(void rlc_um_v9_3_0_test_exchange_delayed_pdus(rlc_um_entity_t *um_txP, rlc_um_entity_t *um_rxP, uint16_t bytes_txP, uint16_t bytes_rxP, signed int time_tx_delayedP,signed int time_rx_delayedP, int is_frame_incrementedP);)
protected_rlc_um_test(void rlc_um_v9_3_0_buffer_delayed_rx_mac_data_ind(struct mac_data_ind* data_ind_rxP, signed int time_tx_delayedP);)
protected_rlc_um_test(void rlc_um_v9_3_0_test_mac_rlc_loop (struct mac_data_ind *data_indP,  struct mac_data_req *data_requestP, int* drop_countP, int *tx_packetsP, int* dropped_tx_packetsP);)
protected_rlc_um_test(void rlc_um_v9_3_0_test_data_ind (module_id_t module_idP, rb_id_t rb_idP, sdu_size_t sizeP,
mem_block_t *sduP);)
protected_rlc_um_test(void rlc_um_v9_3_0_test_tx_rx_10(void);)
protected_rlc_um_test(void rlc_um_v9_3_0_test_tx_rx_5(void);)
public_rlc_um_test(   void rlc_um_v9_3_0_test_print_trace (void);)
public_rlc_um_test(   void rlc_um_v9_3_0_test(void));
#    endif
