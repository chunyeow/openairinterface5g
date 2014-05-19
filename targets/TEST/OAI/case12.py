#******************************************************************************

#  Eurecom OpenAirInterface
#  Copyright(c) 1999 - 2013 Eurecom

#  This program is free software; you can redistribute it and/or modify it
#  under the terms and conditions of the GNU General Public License,
#  version 2, as published by the Free Software Foundation.

#  This program is distributed in the hope it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#  more details.

#  You should have received a copy of the GNU General Public License along with
#  this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

#  The full GNU General Public License is included in this distribution in
#  the file called "COPYING".

#  Contact Information
#  Openair Admin: openair_admin@eurecom.fr
#  Openair Tech : openair_tech@eurecom.fr
#  Forums       : http://forums.eurecom.fsr/openairinterface
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France

#*****************************************************************************

# \file case02.py
# \brief test case 02 for OAI: executions
# \author Navid Nikaein
# \date 2013
# \version 0.1
# @ingroup _test

import time
import random
import log
import openair 
import core
import os


import shutil # copy file 

NUM_UE=2
NUM_eNB=1
NUM_TRIALS=3

PRB=[25]#,50,75,100]
MCS=[4,5]#,7,9,12,15,18,21,24,27]
ANT_TX=2  # 2 
ANT_RX=2  # 2 
PDCCH=2 #, 2, 3, 4
CHANNEL=["N"] # A,B,C,D,E,F,
TX_MODE=2 # 2, 
MIN_SNR=2
MAX_SNR=18
PERF=80
OPT="-L"
FRAME=500

#OPT="-L -d" # 8bit decoder , activate dci decoding at UE



def execute(oai, user, pw, logfile,logdir,debug):
    
    case = '102'
    oai.send('cd $OPENAIR1_DIR;')     
    oai.send('cd SIMULATION/LTE_PHY;')   
    
    try:
        test = '0'
        name = 'Run oai.dlsim.sanity'
        conf = '-a -n 100'
        diag = 'dlsim is not running normally (Segmentation fault / Exiting / FATAL), debugging might be needed'
        trace = logdir + '/log_' + case + test + '_1.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./dlsim.rel8 ' + conf + tee, 'Segmentation fault', 30)
        trace = logdir + '/log_' + case + test + '_2.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./dlsim.rel8 ' + conf + tee, 'Exiting', 30)
        trace = logdir + '/log_' + case + test + '_3.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./dlsim.rel8 ' + conf + tee, 'FATAL', 30)

    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
    
    try:
        test = 1
        MIN_SNR = 2
        name = 'Run oai.dlsim.perf.70%'
        diag = 'no diagnostic is available, check the log file'
        for i in range(len(PRB)):
            for j in range(len(MCS)):
                for k in range(1,ANT_TX):
                    for m in range (1,ANT_RX):
                        for n in range(1,PDCCH):
                            for o in range(len(CHANNEL)):
                                for p in range(1,TX_MODE):
                                    for q in range(MIN_SNR,MAX_SNR): 
                                        #if  if PRB[i] :
                                            
                                        #conf = '-B' + str(PRB[i]) + ' -m'+str(MCS[j]) + ' -y'+str(k) + ' -z'+str(m) +' -c'+str(n) + ' -g'+str(CHANNEL[o]) + ' -x'+str(p) + ' -s'+str(q) + ' -w1.0 -f.1 -n500 -P -O80' #+ OPT  
                                        #trace = logdir + '/time_meas' + '_prb'+str(PRB[i])+'_mcs'+ str(MCS[j])+ '_anttx' + str(k)+ '_antrx' + str(m)  + '_pdcch' + str(n) + '_channel' +str(CHANNEL[o]) + '_tx' +str(p) + '_snr' +str(q)+'.'+case+str(test)+ '.log'
                                        conf = '-B' + str(PRB[i]) + ' -m'+str(MCS[j]) + ' -y'+str(k) + ' -z'+str(m) +' -c'+str(n) + ' -g'+str(CHANNEL[o]) + ' -x'+str(p) + ' -s'+str(q) + ' -w1.0 -f.1 -P -n'+str(FRAME)+' -O'+str(PERF)+' '+ OPT    
                                        trace = logdir + '/time_meas' + '_prb'+str(PRB[i])+'_mcs'+ str(MCS[j])+ '_anttx' + str(k)+ '_antrx' + str(m)  + '_pdcch' + str(n) + '_channel' +str(CHANNEL[o]) + '_tx' +str(p) + '_snr' +str(q)+'.'+case+str(test)+ '.log'
                                        tee = ' 2>&1 | tee ' + trace
                                        match = oai.send_expect_re('./dlsim.rel8 ' + conf + tee, 'passed', 0, 1000)
                                        if debug : 
                                            print conf
                                        if match :
                                            log.ok(case, str(test), name, conf, '', logfile)
                                            MIN_SNR = q # just to speed up the test
                                            test+=1
                                            break; # found the smallest snr
                                        else :
                                            try:  
                                                if os.path.isfile(trace) :
                                                    os.remove(trace)
                                             
                                            except OSError, e:  ## if failed, report it back to the user ##
                                                print ("Error: %s - %s." % (e.filename,e.strerror))
                                        
    except log.err, e:
        log.fail(case, str(test), name, conf, e.value, diag, logfile,trace)
    #else:
    #    log.ok(case, test, name, conf, '', logfile)
        
