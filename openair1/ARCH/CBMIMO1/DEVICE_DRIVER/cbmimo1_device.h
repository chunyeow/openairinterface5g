/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
#ifndef OPENAIR_DEVICE_H
#define OPENAIR_DEVICE_H

#define XILINX_VENDOR 0x10ee
#define XILINX_ID 0x0007
#define GRLIB_VENDOR 0x16e3 
#define GRLIB_ID 0x0210


#define openair_MAJOR 127



//#define openair_writeb(val,port)     pci_config_writel(); //{writeb((ucchar)(val),(ulong)(port)); mb();}
//#define openair_writew(val,port)     //{writew((ushort)(val),(ulong)(port)); mb();}
#define openair_writel(dev,offset,val)     pci_write_config_dword(dev,(int)offset,(unsigned int)val)//{writel((uclong)(val),(ulong)(port)); mb();}

#define openair_readl(dev,offset,val)     pci_read_config_dword(dev,(int)offset,(unsigned int*)val)//{writel((uclong)(val),(ulong)(port)); mb();}

//#define openair_readb(port)  readb(port)
//#define openair_readw(port)  readw(port)
//#define openair_readl(port)  readl(port)





#define openair_IOC_MAGIC         'm'

#define openair_TEST_FPGA                   _IOR(openair_IOC_MAGIC,1,int)
#define openair_START_1ARY_CLUSTERHEAD      _IOR(openair_IOC_MAGIC,2,int)
#define openair_START_2ARY_CLUSTERHEAD      _IOR(openair_IOC_MAGIC,3,int)
#define openair_START_NODE                  _IOR(openair_IOC_MAGIC,4,int)
#define openair_STOP                        _IOR(openair_IOC_MAGIC,5,int)
#define openair_GET_BUFFER                  _IOR(openair_IOC_MAGIC,6,int)
#define openair_GET_CONFIG                  _IOR(openair_IOC_MAGIC,7,int)
#define openair_GET_VARS                    _IOR(openair_IOC_MAGIC,8,int)
#define openair_SET_TX_GAIN                 _IOR(openair_IOC_MAGIC,9,int)
#define openair_SET_RX_GAIN                 _IOR(openair_IOC_MAGIC,10,int)
#define openair_SET_LO_FREQ                 _IOR(openair_IOC_MAGIC,11,int)
#define openair_START_FS4_TEST              _IOR(openair_IOC_MAGIC,12,int)
#define openair_START_OFDM_TEST             _IOR(openair_IOC_MAGIC,13,int)
#define openair_START_QAM16_TEST            _IOR(openair_IOC_MAGIC,14,int)
#define openair_START_QPSK_TEST             _IOR(openair_IOC_MAGIC,15,int)
#define openair_START_IQ_IMPULSES_TEST      _IOR(openair_IOC_MAGIC,16,int)
#define openair_START_REAL_FS4_WITH_DC_TEST _IOR(openair_IOC_MAGIC,17,int)
#define openair_DUMP_CONFIG                 _IOR(openair_IOC_MAGIC,18,int)
#define openair_RX_RF_MODE                  _IOR(openair_IOC_MAGIC,19,int)
#define openair_SET_TCXO_DAC                _IOR(openair_IOC_MAGIC,20,int)
#define openair_GET_PHASE_ESTIMATE          _IOR(openair_IOC_MAGIC,21,int)
#define openair_DO_SYNCH                    _IOR(openair_IOC_MAGIC,22,int)
#define openair_GET_SIGNALS                 _IOR(openair_IOC_MAGIC,23,int)
#define openair_SET_FFT_SCALE               _IOR(openair_IOC_MAGIC,24,int)
#define openair_FFT_TEST                    _IOR(openair_IOC_MAGIC,25,int)
#define openair_START_CHANSOUNDER           _IOR(openair_IOC_MAGIC,26,int)
#define openair_SET_CALIBRATED_RX_GAIN      _IOR(openair_IOC_MAGIC,27,int)
#define openair_START_TX_SIG                _IOR(openair_IOC_MAGIC,28,int)
/* 5 new ioctls for control of new RF prototype chain (K. Khalfallah, March 2007) */
/* Non posted ioctls (generate an Irq to Leon processor) */
#define openair_NEWRF_ADF4108_WRITE_REG     _IOR(openair_IOC_MAGIC,29,int)
#define openair_NEWRF_ADF4108_INIT          _IOR(openair_IOC_MAGIC,30,int)
#define openair_NEWRF_LFSW190410_WRITE_KHZ  _IOR(openair_IOC_MAGIC,31,int)
#define openair_NEWRF_RF_SWITCH_CTRL        _IOR(openair_IOC_MAGIC,32,int)
#define openair_NEWRF_SETTX_SWITCH_GAIN     _IOR(openair_IOC_MAGIC,33,int)
#define openair_NEWRF_SETRX_SWITCH_GAIN     _IOR(openair_IOC_MAGIC,34,int)
/* Posted ioctls (DO NOT generate an Irq to Leon processor - so its firmware
 * should intentionally read back their parameter-values for the corresponding
 * action to complete). */
#define openair_NEWRF_ADF4108_WRITE_REG_POSTED        _IOR(openair_IOC_MAGIC,35,int)
#define openair_NEWRF_LFSW190410_WRITE_KHZ_POSTED     _IOR(openair_IOC_MAGIC,36,int)
#define openair_NEWRF_RF_SWITCH_CTRL_POSTED           _IOR(openair_IOC_MAGIC,37,int)
#define openair_NEWRF_SETTX_SWITCH_GAIN_POSTED        _IOR(openair_IOC_MAGIC,38,int)
#define openair_NEWRF_SETRX_SWITCH_GAIN_POSTED        _IOR(openair_IOC_MAGIC,39,int)

#define openair_UPDATE_FIRMWARE             _IOR(openair_IOC_MAGIC,40,int)
// fkalten 25.9.07 this should enable the recording in multiuser mode  
#define openair_START_EMOS_NODEB            _IOR(openair_IOC_MAGIC,41,int)	
#define openair_config_topology             _IOR(openair_IOC_MAGIC,42,int)
#define openair_stop_emulation              _IOR(openair_IOC_MAGIC,43,int)
// fkalten 11.4.08 set the timing advance
#define openair_SET_TIMING_ADVANCE          _IOR(openair_IOC_MAGIC,44,int)
// rknopp 27.08.08 for AgileRF tests
#define openair_START_TX_SIG_NO_OFFSET                _IOR(openair_IOC_MAGIC,45,int)
// fkalten+ghozzi 6.4.09 for cognitive operation
#define openair_START_1ARY_CLUSTERHEAD_COGNITIVE      _IOR(openair_IOC_MAGIC,46,int)
//fkalten 22.4.09 
#define openair_SET_RX_MODE                           _IOR(openair_IOC_MAGIC,47,int)
// fkalten 9.12.09
#define openair_SET_FREQ_OFFSET                       _IOR(openair_IOC_MAGIC,48,int) 

#define openair_GET_BIGPHYSTOP                        _IOR(openair_IOC_MAGIC,49,int)
#define openair_SET_UE_DL_MCS                         _IOR(openair_IOC_MAGIC,50,int)
#define openair_SET_UE_UL_MCS                         _IOR(openair_IOC_MAGIC,51,int)
#define openair_SET_UE_UL_NB_RB                       _IOR(openair_IOC_MAGIC,52,int)
#define openair_SET_DLSCH_RATE_ADAPTATION             _IOR(openair_IOC_MAGIC,53,int)
#define openair_SET_DLSCH_TRANSMISSION_MODE           _IOR(openair_IOC_MAGIC,54,int)
#define openair_SET_ULSCH_ALLOCATION_MODE             _IOR(openair_IOC_MAGIC,55,int)
#define openair_SET_RRC_CONN_SETUP                    _IOR(openair_IOC_MAGIC,56,int)
#define openair_SET_COOPERATION_FLAG                  _IOR(openair_IOC_MAGIC,57,int)
#define openair_SET_RX_OFFSET                         _IOR(openair_IOC_MAGIC,58,int)
#define openair_START_LXRT                            _IOR(openair_IOC_MAGIC,59,int)
#define openair_GET_PCI_INTERFACE                     _IOR(openair_IOC_MAGIC,60,int)
#define openair_MAXNR         60



#define uclong unsigned int
#define ucshort unsigned short
#define ucchar unsigned char


#define MMAP_SIZE 

/* ---------------------------------------------------------------------- */
#define TEST_FAILURE_ADC_MEM -1000
#define TEST_FAILURE_DAC_MEM -1001
#define TEST_FAILURE_DMA -1002


/* Behold: Added directly in (and only in!) R. Knopp's account
   (K. Khalfallah, May 10th, 2007) */
#define GRPCI_IOCONFIG_CTRL0       0x60
#define GRPCI_IOCONFIG_CTRL1       0x64
#define GRPCI_IOCONFIG_CTRL2       0x68
#define GRPCI_IOCONFIG_CTRL3       0x6c

#define NO_INFINITE_ACK_LOOP              (~0)
/* Update firmware commands */
#define UPDATE_FIRMWARE_TRANSFER_BLOCK    0x1
#define UPDATE_FIRMWARE_CLEAR_BSS         0x2
#define UPDATE_FIRMWARE_START_EXECUTION   0x3
#define UPDATE_FIRMWARE_FORCE_REBOOT      0x4
#define UPDATE_FIRMWARE_TEST_GOK          0x5

#endif /* OPENAIR_DEVICE_H */
