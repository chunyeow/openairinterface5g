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

NUM_UE=2
NUM_eNB=1
NUM_TRIALS=3

def execute(oai, user, pw, logfile):
    
    case = '02'
    oai.send('cd $OPENAIR_TARGETS;')
    oai.send('cd SIMU/USER;')
    
    try:
        test = '00'
        name = 'Run oai.rel8.sf'
        conf = '-a -A AWGN -n 100'
        diag = 'OAI is not running normally (Segmentation fault / Exiting / FATAL), debugging might be needed'
        oai.send_expect_false('./oaisim.rel8 ' + conf, 'Segmentation fault', 30)
        oai.send_expect_false('./oaisim.rel8 ' + conf, 'Exiting', 30)
        oai.send_expect_false('./oaisim.rel8 ' + conf, 'FATAL', 30)

    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '01'
        name = 'Run oai.rel8.err'
        conf = '-a -A AWGN -n 100'
        diag = 'Error(s) found in the execution, check the execution logs'
        oai.send_expect_false('./oaisim.rel8 ' + conf, '[E]', 30)
        
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    try:
        test = '02'
        name = 'Run oai.rel8.abs.rrc'
        diag = 'RRC procedure is not finished completely, check the execution logs and trace BCCH, CCCH, and DCCH channels'
        for i in range(NUM_UE) :
            for j in range(NUM_eNB) :
                conf = '-a -A AWGN -n' + str((i+1+j) * 40) + ' -u' + str(i+1) +' -b'+ str(j+1)
                oai.send_expect('./oaisim.rel8 ' + conf, ' Received RRCConnectionReconfigurationComplete from UE ' + str(i),  (i+1) * 50)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    try:
        test = '03'
        name = 'Run oai.rel8.abs.ping'
        diag = 'Data-plane is not working normally, check the OAI protocol stack, OAI driver, and normal operation of the OS'
        for i in range(NUM_UE) :
            for j in range(NUM_eNB) :
                conf = '-a -A AWGN -u' + str(i+1) +' -b'+ str(j+1)
                if user == 'root' :
                    oai.send_nowait('./oaisim.rel8.nas ' + conf + ' > /dev/null &')
                else :    
                    oai.send_nowait('echo '+pw+ ' | sudo -S -E ./oaisim.rel8.nas ' + conf + ' > /dev/null &')
                time.sleep(2)
                for k in range(NUM_TRIALS) :
                    oai.send_expect('ping 10.0.'+str(j+1)+'.'+str(NUM_eNB+i+1) + ' -c ' +  str(random.randint(2, 10))+ ' -s ' + str(random.randint(128, 1500)), ' 0% packet loss', 300)
                if user == 'root' :
                    oai.send('pkill oaisim;')
                    oai.send('pkill oaisim.rel8.nas;')
                else :
                    oai.send_nowait('echo '+pw+ ' | sudo -S pkill oaisim ;')
                    oai.send_nowait('echo '+pw+ ' | sudo -S pkill oaisim.rel8.nas;')

    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    try:
        test = '04'
        name = 'Run oai.rel8.phy.rrc'
        diag = 'RRC procedure is not finished completely, check the execution logs and trace BCCH, CCCH, and DCCH channels'
        for i in range(NUM_UE) :
            for j in range(NUM_eNB) :
                conf = '-A AWGN -n' + str((i+1+j) * 50) + ' -u' + str(i+1) +' -b'+ str(j+1) + ' -s15 -x1'
                oai.send_expect('./oaisim.rel8 ' + conf, ' Received RRCConnectionReconfigurationComplete from UE ' + str(i),  (i+1) * 100)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '05'
        name = 'Run oai.rel10.abs.rrc'
        diag = 'RRC procedure is not finished completely, check the execution logs and trace BCCH, CCCH, and DCCH channels'
        for i in range(NUM_UE) :
            for j in range(NUM_eNB) :
                conf = '-a -A AWGN -n' + str((i+1+j) * 50) + ' -u' + str(i+1) +' -b'+ str(j+1)
                oai.send_expect('./oaisim.rel10 ' + conf, ' Received RRCConnectionReconfigurationComplete from UE ' + str(i),  (i+1) * 100)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)    

    try:
        test = '06'
        name = 'Run oai.rel10.phy.rrc'
        diag = 'RRC procedure is not finished completely, check the execution logs and trace BCCH, CCCH, and DCCH channels'
        for i in range(NUM_UE) :
            for j in range(NUM_eNB) :
                conf = '-A AWGN -s 15 -x 1 -n' + str((i+1+j) * 50) + ' -u' + str(i+1) +' -b'+ str(j+1)
                oai.send_expect('./oaisim.rel10 ' + conf, ' Received RRCConnectionReconfigurationComplete from UE ' + str(i),  (i+1) * 100)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)    
