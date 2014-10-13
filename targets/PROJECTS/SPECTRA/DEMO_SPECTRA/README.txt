
########################################
#
# SPECTRA DEMO INSTALLATION AND RUN
#
#
#
# Rui Costa <ferreira@eurecom.fr>
#
# v1.12 (13 Oct 2014)
#
########################################

########################################
1. REQUIREMENTS


   To properly run the demo these are the main pieces of software 
   that are required:
   - OPENAIR4G (checkout the main trunk from the EURECOM SVN).
   - ODTONE (Open 802.21 protocol from ITAveiro git server).
     - Boost C++ Libraries are requested by ODTONE (v1.48 or higher)
   - REST Toolkit (Codename "Casablanca")
   - Adequate shell/bash start-up scripts for both eNB and UE.

   BEFORE attempting any installation YOU MUST check your system for 
   any required software that these components require, as for 
   instance, a compatible GCC/G++ compiler, build tools, etc.

   NOTES BEFORE INSTALLATION: 

   N1. Installation SHOULD NOT be done as ROOT. IF and WHEN super 
   user permissions are required you will be prompted for them. 
   
   N2. You MUST have a valid EURECOM SVN-enabled account for the 
       Openair4G code.
   
   N3. For ODTONE software, you MUST have installed the ITAveiro 
       SSL/TLS certificates available at their webserver.

   N4. You MUST have Internet access in order to properly build and 
       deploy the software, as some of the software is fetched online.


########################################
2. BUILD & INSTALL

   2.1. PREPARATION
   Start by creating a folder on a non-root directory, preferably 
   your home folder. Copy the <spectra_demo_prepare.sh> script into 
   the newly created folder, henceforth referred to as 
   <spectra_root_folder>.


   2.2. INSTALLATION
   There are two ways of processing the installation:
   - If you already have performed a checkout of the OpenAir svn. 
     (Aimed at single installations for OAI developers.)
   - If you only have the demonstration source code & scripts.
     (To allow easier deployment over several machines, just copy
     the demo's source folder, including the prepare and install 
     scripts to which ever machine you need and let the scripts do 
     the work for you)


      2.2.1 From the OPENAIR SVN
      Go to the folder: 
      <openair_root_folder>/trunk/targets/PROJECTS/SPECTRA/DEMO_SPECTRA
      and run the installation script with the node type to install:
      # ./spectra_demo_install.sh -oai_svn <node_type>

      2.2.2 From the demo's source code and scripts.
      Go to the <spectra_root_folder> and run:
      # ./spectra_demo_install.sh -full <node_type>
   

   
   If the previous step has been well performed, you should now have
   a local working copy of the OpenAir SVN on your local machine, along 
   with additional code required for the demo.
   

   2.3. CRM
   This component of the demo MUST be compiled and installed separately.
   Check <spectra_root_folder>/CRM/ for more details.

   2.4. TVWS Sensing Measurements
   There is a server and client for simulating the TVWS measurements and
   run the cognitive algorithm to activate the required actions on the 
   LTE link. (this is the goal of the demo)
   Copy the client & server located at <spectra_root_folder>/clientSensing/ 
   into the ENB2 startup folder (<openair_root_folder>/targets/PROJECTS/SPECTRA)

 
   2.5. TROUBLESHOOTING
   If for some reason the installation does not run as expected, check 
   for permission-related issues or missing software packages. 
   Otherwise, try to install the components one by one, as defined in 
   the installation scripts. 
   (see <spectra_demo_prepare.sh> and <spectra_demo_install.sh> for 
   compilation and configuration details).

   The preparation script will automatically call the installation 
   script, but in case it does not, run it manually, it is located at:
   <spectra_root_folder>/openair4G/targets/PROJECTS/SPECTRA/

   NOTE: During installation we apply a patch to ODTONE, due to the 
         lack of a definition for LTE links and their attributes. 
         However, as new versions are released the patch may need to 
         be adapted. Therefore, be aware of compilation issues 
         originating from the compilation of the ODTONE core functions. 


########################################
3. CONFIGURATION

   You MUST make sure that the configuration of your physical and 
   logical interfaces matches with your Open Air and ODTONE 
   configuration files. Take all parameters into account while 
   checking the configuration files and launch scripts, located in:

   <spectra_root_folder>/openair4G/targets/PROJECTS/SPECTRA/

   <spectra_root_folder>/ODTONE/dist/

   3.1 Base Configuration
   This is the network configuration on the config files:

      UE1
      eth0 (192.168.13.2) - oai0 (10.0.0.2) (LTE emulated link)
   
      ENB1 
      eth0 (192.168.12.122) internet access gw (192.168.12.100)
      eth1 (192.168.13.1) - oai0 (10.0.0.1) (LTE emulated link)
      eth2 (192.168.14.3) (Internal Relay Connection)

      UE2
      eth0 (192.168.14.4) (Internal Relay Connection)
      eth1 (192.168.15.5) - oai0 (10.0.2.3) (LTE emulated link)
  
      ENB2
      eth0 (192.168.15.6) - oai0 (10.0.2.4) (LTE emulated link)



########################################
4. RUNNING THE DEMO

   1. We advise you to turn off the network manager service in order 
      for the startup script configuration to be effective and final. 
      This can be achieved by executing:

      # sudo service network-manager stop

   2. (On all machines) Go to OpenAir Launch folder: 
      <spectra_root_folder>/openair4G/targets/PROJECTS/SPECTRA/

   3. Launch the eNB1 on eNB1 machine: 
      # sudo ./start_enb.bash
      Wait for eNB1 to reach stand by mode.

   4. Launch the UE1 on UE1 machine: 
      # sudo ./start_ue.bash
   
   5. Launch the eNB2 on eNB2 machine: 
      # sudo ./start_enb.bash
      Wait for eNB2 to reach stand by mode.
   
   6. Launch the UE2 on UE2 machine: 
      # sudo ./start_ue.bash

   7. Any other modules will be launched automatically 
      if present. (CRM client, TVWS sensing, MIHF, MIH Users, etc.)

   Enjoy the demo!







########################################
CHANGELOG

v0.99 (03/09/2014)
      - First <spectra_demo_prepare.sh> script and <README> file.

v1.00 (04/09/2014)
      - Added compartmentalization for each sw component
      - Added full installation procedure from scratch
      - Fetching sw from repositories, compilation & installation
      - Revisioned output

v1.01 (05/09/2014)
      - Added OpenAir conf files <spectra_demo_src/openair_conf>
      - Added MIHF Users <spectra_demo_src/mih_users>
      - Reduced printed output

v1.02 (08/09/2014)
      - Bug correcting on installation and configuration paths & scripts
      - Patched Open Air script <targets/PROJECTS/SPECTRA/build_all.sh>

v1.03 (09/09/2014)
      - Patched MIHF to support LTE link based on previous code
      - Added <spectra_demo_src/mihf/> 
        with patch files: <link.hpp> <bin_query.hpp>
      - Revisioned <spectra_demo_prepare.sh>

v1.04 (11/09/2014)
      - Corrected MIHF patch bugs
      - Added patch file <archive.hpp>
      - Corrected path and filename bugs on <spectra_demo_prepare.sh>

v1.05 (15/09/2014)
      - Corrected MIHF patch bugs
      - Added a number of patch files
      - Adjusted patch execution on <spectra_demo_prepare.sh>

v1.06 (17/09/2014)
      - Working demo 1 eNB 1 UE
      - Modified configuration files for UE and eNB
      - Substituted overwrite of files into a Git patch for SPECTRA

v1.07 (19/09/2014)
      - Expanded demo to 4 machines:
        - 2 real: ue1, eNB1 
        - 2 virtual: eNB2, ue2 (both hosted by eNB1)
      
v1.08 (23/09/2014)
      - Included CRM client and TVWS sensing modules.

v1.09 (24/09/2014)
      - Final Demo setup ready.

v1.10 (29/09/2014)
      - Updated RRC code for OpenAir.
      - Code commit to OpenAir SVN trunk.

v1.11 (01/10/2014)
      - Restructure of source code tree and installation files to 
        include the full demo (2 eNB and 2 ue)
      - Splitted prepare and install scripts.
      - Added colour coded steps and error stoppage on install script.
      
v1.12 (13/10/2014)
      - Added bug fixes on launch and install scripts
      - Added final version to openair svn
      - Reworked README file.


