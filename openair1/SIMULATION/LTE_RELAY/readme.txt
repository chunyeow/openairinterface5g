
Author : Erhan YILMAZ
Contact: erhan.yilmaz@eurecom.fr


Function: relay_DF_sim.c

In this function we intend to simulate Paralel Relay Network scenario where a single source node (eNB in the simulator) communicates with a destination node via multiple relay nodes (RNs) which are connected to the destination node via finite (but sufficent to convey decode bits) capacity backhaul links.  

Each RN perform FULL decoding (i.e., decode-and-forward relaying). 

- Maxumim HARQ retransmision rounds is set to 4; 
- An error declared, if one of the following occurs:
	1- For all RNs DCI pkts are not received correctly
	2- Assuming correct DCI pkts but maxumum number of iterations for the turbo decoders at at least one of the RNs (among those who have correctly got the DCI pkts) is exceeded;
//	3- Maximum number of HARQ iterations is axceeded. //   
