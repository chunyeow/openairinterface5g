/*
                                 pdcp.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#ifndef __PDCP_H__
#    define __PDCP_H__
//-----------------------------------------------------------------------------
#    ifdef PDCP_C
#        define private_pdcp(x) x
#        define protected_pdcp(x) x
#        define public_pdcp(x) x
#    else
#        define private_pdcp(x)
#        define public_pdcp(x) extern x
#        ifdef PDCP_FIFO_C
#            define protected_pdcp(x) extern x
#        else
#            define protected_pdcp(x)
#        endif
#    endif

#    ifdef PDCP_FIFO_C
#        define private_pdcp_fifo(x) x
#        define protected_pdcp_fifo(x) x
#        define public_pdcp_fifo(x) x
#    else
#        define private_pdcp_fifo(x)
#        define public_pdcp_fifo(x) extern x
#        ifdef PDCP_C
#            define protected_pdcp_fifo(x) extern x
#        else
#            define protected_pdcp_fifo(x)
#        endif
#    endif
//-----------------------------------------------------------------------------
#ifndef NON_ACCESS_STRATUM
//#include "rtos_header.h"
//#include "openair_defs.h"
//#include "platform_types.h"
//#include "platform_constants.h"
#include "UTIL/MEM/mem_block.h"
#include "UTIL/LISTS/list.h"
#include "COMMON/mac_rrc_primitives.h"
#endif //NON_ACCESS_STRATUM
//-----------------------------------------------------------------------------

public_pdcp(unsigned int Pdcp_stats_tx[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_tx_bytes[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_tx_bytes_last[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_tx_rate[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_rx[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_rx_bytes[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_rx_bytes_last[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_rx_rate[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);

public_pdcp(void pdcp_data_req       (module_id_t, rb_id_t, sdu_size_t, char*);)
public_pdcp(void pdcp_data_ind       (module_id_t, rb_id_t, sdu_size_t, mem_block_t*);)
public_pdcp(void pdcp_config_req     (module_id_t, rb_id_t);)
public_pdcp(void pdcp_config_release (module_id_t, rb_id_t);)


public_pdcp(void pdcp_run (void);)
public_pdcp(int pdcp_module_init ();)
public_pdcp(void pdcp_module_cleanup ();)
public_pdcp(void pdcp_layer_init ();)
public_pdcp(void pdcp_layer_cleanup ();)

#define PDCP2NAS_FIFO 21
#define NAS2PDCP_FIFO 22

protected_pdcp_fifo(int pdcp_fifo_flush_sdus ();)
protected_pdcp_fifo(int pdcp_fifo_read_input_sdus_remaining_bytes ();)
protected_pdcp_fifo(int pdcp_fifo_read_input_sdus();)
//-----------------------------------------------------------------------------

typedef struct pdcp_data_req_header_t {
  rb_id_t             rb_id;
  sdu_size_t           data_size;
  int       inst;
} pdcp_data_req_header_t;

typedef struct pdcp_data_ind_header_t {
  rb_id_t             rb_id;
  sdu_size_t           data_size;
  int       inst;
} pdcp_data_ind_header_t;

typedef struct pdcp_t {
  char              allocation;
  // here ROHC variables for header compression/decompression
} pdcp_t;
protected_pdcp(signed int             pdcp_2_nas_irq;)
protected_pdcp(pdcp_t                 pdcp_array[MAX_MODULES][MAX_RAB];)
protected_pdcp(sdu_size_t             pdcp_output_sdu_bytes_to_write;)
protected_pdcp(sdu_size_t             pdcp_output_header_bytes_to_write;)
protected_pdcp(list_t                 pdcp_sdu_list;)
protected_pdcp(int                    pdcp_sent_a_sdu;)
protected_pdcp(pdcp_data_req_header_t pdcp_input_header;)
protected_pdcp(char                   pdcp_input_sdu_buffer[MAX_IP_PACKET_SIZE];)
protected_pdcp(sdu_size_t             pdcp_input_index_header;)
protected_pdcp(sdu_size_t             pdcp_input_sdu_size_read;)
protected_pdcp(sdu_size_t             pdcp_input_sdu_remaining_size_to_read;)
#endif
