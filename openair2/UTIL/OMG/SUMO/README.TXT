----------------------------------------------------------------------------------
OpenAir Mobility Generator (OMG) - Mobility provided by the SUMO simulator
----------------------------------------------------------------------------------

Notes: 

  - The Simulation of Urban Mobility (SUMO) needs to be downloaded, configured and compiled. Please download the sources from this address 
   
       http://sumo.sourceforge.net/

    or alternatively chechout from the following SUMO svn
       
       svn co https://sumo.svn.sourceforge.net/svnroot/sumo/trunk/sumo sumo 

       (among the other required packages for SUMO, you also need the following tools for SUMO compilation. Please use the following command to install: 
       sudo apt-get install build-essential autoconf buildtool)

     and follow the installation instructions at http://sumo.sourceforge.net/doc/current/docs/userdoc/Installing.html

  - OMG will look for the SUMO and SUMO-GUI exectuable from this SUMO folder.

  - SUMO/SCENARIOS is the default folder for SUMO scenarios. Please add all required scenario on that folder. 

  - OMG will launch SUMO with one single SUMO config file. Please regoup all your SUMO XML files (network, route et others) in one .cfg file. More details on the SUMO website. 

  - SUMO will be lauched using the following command

      sumo -c scen.sumo.cfg -b 0 -e 1000 -v

    or, if you want to enable TraCI server (here as example listening on port 8883). Please bear in mind that this launch will be blocking upon a TraCI client

      sumo -c scen.sumo.cfg -b 0 -e 1000 -remote-port 8883 -v
    
    or, if you prefer the SUMO GUI and select manually the scenario file on the GUI window

      sumo-gui 


   - Once installation completed, please check that SUMO is working properly by following the STANDALONE scenario 
     - GUI: lauch: sumo-gui 
            load: scen.sumo.cfg

     - CommandLine: sumo -c scen.sumo.cfg -b 0 -e 1000 -v

----------------------------------------------------------------------------------
The OpenAirInterface Team 
openair_tech@eurecom.fr
----------------------------------------------------------------------------------
