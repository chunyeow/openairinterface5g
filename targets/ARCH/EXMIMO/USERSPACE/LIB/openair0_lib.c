/*e openair0_lib : API to interface with ExpressMIMO-1&2 kernel driver
 * 
 *  Authors: Matthias Ihmig <matthias.ihmig@mytum.de>, 2013
 *           Raymond Knopp <raymond.knopp@eurecom.fr>
 * 
 *  Changelog:
 *  28.01.2013: Initial version
 */

#include <fcntl.h> 
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "openair0_lib.h"
#include "openair_device.h"
#include "common_lib.h"
#define max(a,b) ((a)>(b) ? (a) : (b))
exmimo_pci_interface_bot_virtual_t openair0_exmimo_pci[MAX_CARDS]; // contains userspace pointers for each card

char *bigshm_top[MAX_CARDS] = INIT_ZEROS;

int openair0_fd;
int openair0_num_antennas[MAX_CARDS];
int openair0_num_detected_cards = 0;

unsigned int PAGE_SHIFT;


unsigned int log2_int( unsigned int x )
{
  unsigned int ans = 0 ;
  while( x>>=1 ) ans++;
  return ans ;
}

int openair0_open(void)
{
    exmimo_pci_interface_bot_virtual_t exmimo_pci_kvirt[MAX_CARDS];
    unsigned int bigshm_top_kvirtptr[MAX_CARDS];
    
    int card;
    int ant;
    int openair0_num_antennas[4];

    PAGE_SHIFT = log2_int( sysconf( _SC_PAGESIZE ) );
    
    if ((openair0_fd = open("/dev/openair0", O_RDWR,0)) <0)
    {
        return -1;
    }

    ioctl(openair0_fd, openair_GET_NUM_DETECTED_CARDS, &openair0_num_detected_cards);
    
    if ( openair0_num_detected_cards == 0 )
    {
        fprintf(stderr, "No cards detected!\n");
        return -4;
    }

    ioctl(openair0_fd, openair_GET_BIGSHMTOPS_KVIRT, &bigshm_top_kvirtptr[0]);
    ioctl(openair0_fd, openair_GET_PCI_INTERFACE_BOTS_KVIRT, &exmimo_pci_kvirt[0]);
    
    //printf("bigshm_top_kvirtptr: %08x  %08x  %08x  %08x\n", bigshm_top_kvirtptr[0], bigshm_top_kvirtptr[1], bigshm_top_kvirtptr[2], bigshm_top_kvirtptr[3]);
    
    for( card=0; card < openair0_num_detected_cards; card++)
    {
        bigshm_top[card] = (char *)mmap( NULL,
                           BIGSHM_SIZE_PAGES<<PAGE_SHIFT,
                           PROT_READ|PROT_WRITE,
                           MAP_SHARED, //|MAP_FIXED,//MAP_SHARED,
                           openair0_fd,
                           ( openair_mmap_BIGSHM | openair_mmap_Card(card) )<<PAGE_SHIFT);

        if (bigshm_top[card] == MAP_FAILED) {
            openair0_close();
            return -2;
        }

        // calculate userspace addresses
        openair0_exmimo_pci[card].firmware_block_ptr = (char*) (bigshm_top[card] +  (unsigned int)exmimo_pci_kvirt[0].firmware_block_ptr - bigshm_top_kvirtptr[0]);
        openair0_exmimo_pci[card].printk_buffer_ptr  = (char*) (bigshm_top[card] +  (unsigned int)exmimo_pci_kvirt[0].printk_buffer_ptr  - bigshm_top_kvirtptr[0]);
        openair0_exmimo_pci[card].exmimo_config_ptr  = (exmimo_config_t*) (bigshm_top[card] +  (unsigned int)exmimo_pci_kvirt[0].exmimo_config_ptr  - bigshm_top_kvirtptr[0]);
        openair0_exmimo_pci[card].exmimo_id_ptr      = (exmimo_id_t*)     (bigshm_top[card] +  (unsigned int)exmimo_pci_kvirt[0].exmimo_id_ptr      - bigshm_top_kvirtptr[0]);

        //printf("openair0_exmimo_pci.firmware_block_ptr (%p) =  bigshm_top(%p) + exmimo_pci_kvirt.firmware_block_ptr(%p) - bigshm_top_kvirtptr(%x)\n",
        //    openair0_exmimo_pci[card].firmware_block_ptr, bigshm_top, exmimo_pci_kvirt[card].firmware_block_ptr, bigshm_top_kvirtptr[card]);
        //printf("card%d, openair0_exmimo_pci.exmimo_id_ptr      (%p) =  bigshm_top(%p) + exmimo_pci_kvirt.exmimo_id_ptr     (%p) - bigshm_top_kvirtptr(%x)\n",
        //    card, openair0_exmimo_pci[card].exmimo_id_ptr, bigshm_top[card], exmimo_pci_kvirt[card].exmimo_id_ptr, bigshm_top_kvirtptr[card]);
        
  //if (openair0_exmimo_pci[card].exmimo_id_ptr->board_swrev != BOARD_SWREV_CNTL2)
 //    {
//       error("Software revision %d and firmware revision %d do not match, Please update either Software or Firmware",BOARD_SWREV_CNTL2,openair0_exmimo_pci[card].exmimo_id_ptr->board_swrev);
//       return -5; 
 //    }
    
        if ( openair0_exmimo_pci[card].exmimo_id_ptr->board_exmimoversion == 1)
            openair0_num_antennas[card] = 2;

        if ( openair0_exmimo_pci[card].exmimo_id_ptr->board_exmimoversion == 2)
            openair0_num_antennas[card] = 4;


        for (ant=0; ant<openair0_num_antennas[card]; ant++)
        {
            openair0_exmimo_pci[card].rxcnt_ptr[ant] = (unsigned int *) (bigshm_top[card] +  (unsigned int)exmimo_pci_kvirt[card].rxcnt_ptr[ant] - bigshm_top_kvirtptr[card]);
            openair0_exmimo_pci[card].txcnt_ptr[ant] = (unsigned int *) (bigshm_top[card] +  (unsigned int)exmimo_pci_kvirt[card].txcnt_ptr[ant] - bigshm_top_kvirtptr[card]);
        }

        for (ant=0; ant<openair0_num_antennas[card]; ant++)
        {
            openair0_exmimo_pci[card].adc_head[ant] = mmap( NULL,
                       ADAC_BUFFERSZ_PERCHAN_B,
                       PROT_READ|PROT_WRITE,
                       MAP_SHARED, //|MAP_FIXED,//MAP_SHARED,
                       openair0_fd,
                       ( openair_mmap_RX(ant) | openair_mmap_Card(card) )<<PAGE_SHIFT );

            openair0_exmimo_pci[card].dac_head[ant] = mmap( NULL,
                       ADAC_BUFFERSZ_PERCHAN_B,
                       PROT_READ|PROT_WRITE,
                       MAP_SHARED, //|MAP_FIXED,//MAP_SHARED,
                       openair0_fd,
                       ( openair_mmap_TX(ant) | openair_mmap_Card(card) )<<PAGE_SHIFT );
                       
            if (openair0_exmimo_pci[card].adc_head[ant] == MAP_FAILED || openair0_exmimo_pci[card].dac_head[ant] == MAP_FAILED) {
                openair0_close();
                return -3;
            }
        }

        //printf("p_exmimo_config = %p, p_exmimo_id = %p\n", openair0_exmimo_pci.exmimo_config_ptr, openair0_exmimo_pci.exmimo_id_ptr);
        
        printf("card %d: ExpressMIMO %d, HW Rev %d, SW Rev 0x%d, %d antennas\n", card, openair0_exmimo_pci[card].exmimo_id_ptr->board_exmimoversion,
            openair0_exmimo_pci[card].exmimo_id_ptr->board_hwrev, openair0_exmimo_pci[card].exmimo_id_ptr->board_swrev, openair0_num_antennas[card]);
      
    } // end for(card)
    return 0;
}
    
    
int openair0_close(void)
{
    int ant;
    int card;
    
    close(openair0_fd);
    
    for (card=0; card<openair0_num_detected_cards; card++)
    {
        if (bigshm_top[card] != NULL && bigshm_top[card] != MAP_FAILED)
            munmap(bigshm_top[card], BIGSHM_SIZE_PAGES<<PAGE_SHIFT);

        for (ant=0; ant<openair0_num_antennas[card]; ant++)
        {
            if (openair0_exmimo_pci[card].adc_head[ant] != NULL && openair0_exmimo_pci[card].adc_head[ant] != MAP_FAILED)
                munmap(openair0_exmimo_pci[card].adc_head[ant], ADAC_BUFFERSZ_PERCHAN_B);

            if (openair0_exmimo_pci[card].dac_head[ant] != NULL && openair0_exmimo_pci[card].dac_head[ant] != MAP_FAILED)
                munmap(openair0_exmimo_pci[card].dac_head[ant], ADAC_BUFFERSZ_PERCHAN_B);
        }
    }
    return 0;
}

int openair0_dump_config(int card)
{
    return ioctl(openair0_fd, openair_DUMP_CONFIG, card);
}

int openair0_get_frame(int card)
{
    return ioctl(openair0_fd, openair_GET_FRAME, card);
}

int openair0_start_rt_acquisition(int card)
{
    return ioctl(openair0_fd, openair_START_RT_ACQUISITION, card);
}

int openair0_stop(int card)
{
    return ioctl(openair0_fd, openair_STOP, card);
}

int openair0_stop_without_reset(int card)
{
    return ioctl(openair0_fd, openair_STOP_WITHOUT_RESET, card);
}

static exmimo_config_t         *p_exmimo_config;
static exmimo_id_t             *p_exmimo_id;
#define MY_RF_MODE      (RXEN + TXEN + TXLPFNORM + TXLPFEN + TXLPF25 + RXLPFNORM + RXLPFEN + RXLPF25 + LNA1ON +LNAMax + RFBBNORM + DMAMODE_RX + DMAMODE_TX)
#define RF_MODE_BASE    (TXLPFNORM + TXLPFEN + TXLPF25 + RXLPFNORM + RXLPFEN + RXLPF25 + LNA1ON +LNAMax + RFBBNORM)

int openair0_device_init(openair0_device *device, openair0_config_t *openair0_cfg) {

  // Initialize card
  int ret;
  int ant;

  ret = openair0_open();
  if ( ret != 0 ) {
    if (ret == -1)
      printf("Error opening /dev/openair0");
    if (ret == -2)
      printf("Error mapping bigshm");
    if (ret == -3)
      printf("Error mapping RX or TX buffer");
    return(ret);
  }

  printf ("Detected %d number of cards, %d number of antennas.\n", openair0_num_detected_cards, openair0_num_antennas[0]);
  
  p_exmimo_config = openair0_exmimo_pci[0].exmimo_config_ptr;
  p_exmimo_id     = openair0_exmimo_pci[0].exmimo_id_ptr;
  
  printf("Card %d: ExpressMIMO %d, HW Rev %d, SW Rev 0x%d\n", 0, p_exmimo_id->board_exmimoversion, p_exmimo_id->board_hwrev, p_exmimo_id->board_swrev);

  // check if the software matches firmware
  if (p_exmimo_id->board_swrev!=BOARD_SWREV_CNTL2) {
    printf("Software revision %d and firmware revision %d do not match. Please update either the firmware or the software!\n",BOARD_SWREV_CNTL2,p_exmimo_id->board_swrev);
    exit(-1);
  }

  if (p_exmimo_id->board_swrev>=9)
    p_exmimo_config->framing.eNB_flag   = 0; 
  else 
    p_exmimo_config->framing.eNB_flag   = 1;//!UE_flag;

  p_exmimo_config->framing.tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_LSB;
#if (BOARD_SWREV_CNTL2>=0x0A)
  for (ant=0; ant<4; ant++)
    p_exmimo_config->framing.resampling_factor[ant] = 2;
#else
    p_exmimo_config->framing.resampling_factor = 2;
#endif

  for (ant=0;ant<max(openair0_cfg->tx_num_channels,openair0_cfg->rx_num_channels);ant++) 
    p_exmimo_config->rf.rf_mode[ant] = RF_MODE_BASE;
  for (ant=0;ant<openair0_cfg->tx_num_channels;ant++)
    p_exmimo_config->rf.rf_mode[ant] += (TXEN + DMAMODE_TX);
  for (ant=0;ant<openair0_cfg->rx_num_channels;ant++) {
    p_exmimo_config->rf.rf_mode[ant] += (RXEN + DMAMODE_RX);
    switch (openair0_cfg->rxg_mode[ant]) {
    default:
    case max_gain:
      p_exmimo_config->rf.rf_mode[ant] = (p_exmimo_config->rf.rf_mode[ant]&(~LNAGAINMASK))|LNAMax;
      break;
    case med_gain:
      p_exmimo_config->rf.rf_mode[ant] = (p_exmimo_config->rf.rf_mode[ant]&(~LNAGAINMASK))|LNAMed;
      break;
    case byp_gain:
      p_exmimo_config->rf.rf_mode[ant] = (p_exmimo_config->rf.rf_mode[ant]&(~LNAGAINMASK))|LNAByp;
      break;
    }
  }
  for (ant=max(openair0_cfg->tx_num_channels,openair0_cfg->rx_num_channels);ant<4;ant++) {
    p_exmimo_config->rf.rf_mode[ant] = 0;
  }
  
  for (ant = 0; ant<openair0_cfg->rx_num_channels; ant++) { 
    p_exmimo_config->rf.do_autocal[ant] = 1;
    p_exmimo_config->rf.rf_freq_rx[ant] = (unsigned int)openair0_cfg->rx_freq[ant];
    p_exmimo_config->rf.tx_gain[ant][0] = (unsigned int)openair0_cfg->rx_gain;
  }
  for (ant = 0; ant<openair0_cfg->tx_num_channels; ant++) { 
    p_exmimo_config->rf.do_autocal[ant] = 1;
    p_exmimo_config->rf.rf_freq_tx[ant] = (unsigned int)openair0_cfg->tx_freq[ant];
    p_exmimo_config->rf.tx_gain[ant][0] = (unsigned int)openair0_cfg->tx_gain;
  }

}

unsigned int *openair0_daq_cnt() {

  return((unsigned int *)openair0_exmimo_pci[0].rxcnt_ptr[0]);

}
