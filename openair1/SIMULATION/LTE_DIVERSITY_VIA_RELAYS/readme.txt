This file contains the details of the simulators used for BLER  and Throughput measurements for a 2 Relay Distributed System:

1. relays_sim: To plot the overall BLER vs Uplink SNR, Uplink BLER vs Uplink SNR for a fixed value of downlink SNR. It also plots Throughput vs Uplink SNR for the entire system.

2. relays_sim_harq: HARQ Scheme 1: First the downlink phase transmits,even if one of the two relays decode, the signal is forwarded to destination otherwise retransmission at phase 1. If destination do not decode correctly, then retransmissions in the phase 2 take place.

It implements harq scheme 1 for cooperative schemes and plots the Uplink BLER vs Uplink SNR, overall BLER vs Uplink SNR and overall Throughput vs Uplink SNR.

3. relays_sim_harq2: HARQ Scheme 2: First the downlink phase transmits,even if one of the two relays decode, the signal is forwarded to destination otherwise retransmission at phase 1.If destination do not decode correctly, then two cases arise: If both the relays had decoded correctly, then retransmission in phase 2 takes place otherwise if only 1 relay had forwarded then retransmission takes place in phase 1.

It implements harq scheme 2 for cooperative schemes and plots the Uplink BLER vs Uplink SNR, overall BLER vs Uplink SNR and overall Throughput vs Uplink SNR.


Flags to change: 1. COLLABRATIVE_SCHEME: If not set, then the simulator is for a single relay case. If set, then it performs cooperative schemes
                 2. relay_flag: If 1, then only one relay. If 2, then 2 relays   
                 3. diversity_scheme: If 0, then no diversity scheme. If 1, then Delay Diversity Scheme and if 2, then Distributed Alamouti Scheme
     
 
 
