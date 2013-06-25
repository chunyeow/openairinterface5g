#include "SCHED/phy_procedures_emos.h"

#define TERM_MODE_SINGL 	0
#define TERM_MODE_MULTI 	1

#define TERM_ON 					1
#define TERM_OFF 					0

#define SCREEN_COLOR_ON		FL_CHARTREUSE	
#define SCREEN_COLOR_OFF	FL_RIGHT_BCOL
#define SCREEN_COLOR_BG		FL_BLACK
#define SCREEN_COLOR_HL		FL_RED
		
#define LED_COLOR_ON			FL_RED
#define LED_COLOR_OFF			FL_RIGHT_BCOL

#define REC_ON 						1
#define REC_OFF 					0

#define EMOS_NOT_READY		0
#define EMOS_READY				1

#define TIME_DOMAIN				0
#define FREQ_DOMAIN				1

#define N0				0
#define SNR				1

#define REC_FRAMES_MAX 22500
#define REC_FRAMES_PER_FILE 18750
#define REC_FILE_IDX_MAX 100

#define CHANSOUNDER_FIFO_DEV "/dev/rtf3"

#define NO_ESTIMATES_DISK 100 //No. of estimates that are aquired before dumped to disk

/*
#define TIMESTAMP_SIZE 8 
#define PDU_ERRORS_SIZE 4 
#define RX_RSSI_SIZE 2 
#define COMPLEX16_SIZE 4 
#define NO_OF_OFDM_CARRIERS 256
#define CHANNEL_BUFFER_SIZE (NB_ANTENNAS*COMPLEX16_SIZE*NO_OF_OFDM_CARRIERS+TIMESTAMP_SIZE+PDU_ERRORS_SIZE+RX_RSSI_SIZE) //in bytes
*/

#define CHANNEL_BUFFER_SIZE sizeof(fifo_read_emos) //in bytes

#define SCREEN_MEMORY_SIZE 32

#define DISP_MAX_POWER 4194304 //maximum value for the display of the frequency response

#define GPSD_BUFFER_SIZE 64

//struct gps_data_struct {}
