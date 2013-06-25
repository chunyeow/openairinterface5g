/*******************************************************************************

  OPENAIRINTERFACE 0/1/2/3 
  Copyright(c) 1999 - 2010 EURECOM

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information:
  Openair Admin <openair_admin@eurecom.fr>
  Mailing List <openair_tech@eurecom.fr>
  Forums  http://forums.eurecom.fr/openairinterface
  Eurecom, 2229, route des cretes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/
/*________________________OEPNAIR/OPENAIR0/________________________

 File    : Readme.txt 
 Authors : navid nikaein
 Company : EURECOM
 Emails  : navid.nikaein@eurecom.fr
________________________________________________________________*/


-------------------------
Table of contents
-------------------------

The content of this readme is the following: 

  1) What is this block about in OpenAirInterface
  2) Folders and files description
  3) Organization of the folders and their dependancies
  4) Makefile targets and how to build
  5) How to use through a tutorial: run a simple experimentation

------------------------------------------------
1) What is this block about in OpenAirInterface ?
-----------------------------------------------
     

     It essentially means "the hardware part" located at openair0.

     More information about ???  can be found on the Twiki:
     https://twiki.eurecom.fr/twiki/bin/view/OpenAirInterface/WebHome

-----------------------------------------
2) Folders and files description
-----------------------------------------

   File/Folder        Description
    -----------        -----------
    
    - Folder1/       contains 
    
    - Folder2/       contains 
    
    - init.bash          A simple script file, that any user SHOULD source from 
    		       its environment personal set-up script (this is typically ~/.bashrc) 
		       by adding the following lines to it:

                        # This is my ~/.bashrc file or equivalent
                         export OPENAIRX=path/to/my/openairX/folder
                         source $OPENAIRX/init.bash
    
                       The file includes variable definitions & path settings to
                       access softwares, sources & Makefiles, software
                       distribution in openair, and so on.

                       YOU DEFINETELY NEED TO SOURCE THIS FILE.

----------------------------------------------------
3) Organization of the folders and their dependancies
----------------------------------------------------

     The 3 folders have the following structures

     - Folder1/subfolder1/  contains any ressource related to the design

     - Folder2/subfolder2/ contains 

     - Folder2/src/ 

     - Folder2/lib/ 

     - Folder2/bin/ 

      Explain where are the source files.  
      Explain the relationship with other Blocks.

    
-----------------------------------------
4)  Makefile targets and how to build
-----------------------------------------

     Explain whether you are using symbolic links or not,
     how to backup/archive
     how to generate the tags
     how to print the vars
     how to create the documentation 

----------------------------------------------------------------
6)  How to use through a tutorial: run a simple experimentation
----------------------------------------------------------------

      ...

