#ifndef __DOT11DEFS_H__
#define __DOT11DEFS_H__

#include <stdint.h>

#define MAX_SDU_SIZE 4095
#define AMP 1024

#define ONE_OVER_SQRT_2 23170
#define ONE_OVER_SQRT_10 10362
#define ONE_OVER_SQRT_42 7150

typedef enum {
  BPSK_1_2=0,
  BPSK_3_4=1,
  QPSK_1_2=2,
  QPSK_3_4=3,
  QAM16_1_2=4,
  QAM16_3_4=5,
  QAM64_1_2=6,
  QAM64_3_4=7
} RATE_t;


typedef struct {
  int sdu_length;
  RATE_t rate;
  int service;
} TX_VECTOR_t;

typedef enum {
  BUSY = 0,
  IDLE = 1
} CHANNEL_STATUS_t;

typedef TX_VECTOR_t RX_VECTOR_t;

int phy_tx_start(TX_VECTOR_t *tx_vector,uint32_t *tx_frame,uint32_t next_TXop_offset,int frame_length,uint8_t *data_ind);
int phy_tx_start_bot(TX_VECTOR_t *tx_vector,int16_t *output_ptr,uint32_t next_TXop_offset,int frame_length,uint8_t *data_ind);

CHANNEL_STATUS_t initial_sync(RX_VECTOR_t **rx_vector,int *rx_offset,int *log2_maxh,uint32_t *rx_frame,int rx_frame_length,int initial_sample_offset,int one_shot);
int data_detection(RX_VECTOR_t *rxv,uint8_t *data_ind,uint32_t* rx_data,int frame_length,int rx_offset,int log2_maxh,int (*wait(int,int)));

void print_is_stats(void);
void print_dd_stats(void);
#endif
