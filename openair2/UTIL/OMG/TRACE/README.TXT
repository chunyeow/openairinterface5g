----------------------------------------------------------------------------------
OpenAir Mobility Generator (OMG) - Mobility provided by trace files
----------------------------------------------------------------------------------
 The trace-driven OMG behaves as follows :
  When we have all the mobility description (the .tr file) in memory sorted in an ascending order of time for all nodes (hash table),  we load one location and we determine the travel time and we move the nodes (update_nodes and get_current_positions act here), once the node is near to the destination (journeytime - eps) next position and time is obtained. If the next time is very far from current time then we put the node to sleep till the next start time.

Notes: 

  - The OMG/TRACE folder is the default folder for OAI and OMG to load mobility
    files. Please place all your files on that folder.

  - OMG TRACE support mobility input given in the following format:

       TIME(s) NODE_ID NODE_POS_X NODE_POS_Y NODE_SPEED(m/s)
   
    Note: OMG uses the speed (NODE_SPEED) at this time (TIME) from this position (NODE_POS_X,NODE_POS_Y) for this NODE_ID 
  - each input may be ordered (in time) or not

  - The mobility described in the mobility file strictly follows the simulation time, only condition where it doesn't follow is, when a node fails to reach its next point (in time) from where it has to start next journey.
 
----------------------------------------------------------------------------------
The OpenAirInterface Team 
openair_tech@eurecom.fr
----------------------------------------------------------------------------------
