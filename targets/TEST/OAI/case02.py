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

def execute(oai, user, pw, host, logfile,logdir,debug):
    
    case = '02'
    oai.send('cd $OPENAIR_TARGETS;')
    oai.send('cd SIMU/USER;')
    
    try:
        test = '00'
        name = 'Run oai.rel8.sf'
        conf = '-a -A AWGN -n 100'
        diag = 'OAI is not running normally (Segmentation fault / Exiting / FATAL), debugging might be needed'
        trace = logdir + '/log_' + host + case + test + '_1.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./oaisim.rel8.' + host + ' ' + conf + tee, 'Segmentation fault', 30)
        trace = logdir + '/log_' + host + case + test + '_2.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./oaisim.rel8.' + host + ' ' + conf + tee, 'Exiting', 30)
        trace = logdir + '/log_' + host + case + test + '_3.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./oaisim.rel8.' + host + ' ' + conf + tee, 'FATAL', 30)

    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '01'
        name = 'Run oai.rel8.err'
        conf = '-a -A AWGN -n 100 -l7'
        trace = logdir + '/log_' + host + case + test + '_3.txt;'
        tee = ' 2>&1 | tee ' + trace
        diag = '[E] Error(s) found during the execution, check the execution logs'
        oai.send_expect_false('./oaisim.rel8.'+ host + ' ' + conf, '[E]', 30)
        
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    try:
        test = '02'
        name = 'Run oai.rel8.abs.rrc'
        diag = 'RRC procedure is not finished completely, check the execution logs and trace BCCH, CCCH, and DCCH channels'
        for i in range(NUM_UE) :
            for j in range(NUM_eNB) :
                conf = '-a -A AWGN -l7 -n' + str((i+1+j) * 50) + ' -u' + str(i+1) +' -b'+ str(j+1)
                trace = logdir + '/log_' + host + case + test + '_' + str(i) + str(j) + '.txt'
                tee = ' 2>&1 | tee ' + trace
                oai.send_expect('./oaisim.rel8.' + host + ' ' + conf + tee, ' Received RRCConnectionReconfigurationComplete from UE ' + str(i),  (i+1) * 50)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    try:
        test = '03'
        name = 'Run oai.rel8.abs.ping'
        diag = 'Data-plane is not working normally, check the OAI protocol stack, OAI driver, and normal operation of the OS'
        
        oai.driver(oai,user,pw)

        for i in range(NUM_eNB) :
            for j in range(NUM_UE) :
                conf = '-a -A AWGN -l7 -u' + str(j+1) +' -b'+ str(i+1)
                trace = logdir + '/log_' + host + case + test + '_' + str(i) + str(j) + '.txt'
                tee = ' 2>&1 > ' + trace

                if user == 'root' :
                    oai.send('./oaisim.rel8.nas.' + host + ' ' + conf + ' &')
                else :    
                    oai.send('echo '+pw+ ' | sudo -S -E ./oaisim.rel8.nas.'+ host + ' ' + conf + tee + ' &')
                time.sleep(10)
                for k in range(NUM_TRIALS) :
                    trace_ping = logdir + '/log_' + host + case + test + '_' + str(i) + str(j) + str(k) + '_ping.txt'
                    tee_ping = ' 2>&1 | tee ' + trace_ping

                    oai.send_expect('ping 10.0.'+str(j+1)+'.'+str(NUM_eNB+i+1) + ' -c ' +  str(random.randint(2, 10))+ ' -s ' + str(random.randint(128, 1500)) + tee_ping, ' 0% packet loss', 20)
                if user == 'root' :
                    oai.send('pkill oaisim.rel8.nas.'+host)
                    oai.send('pkill oaisim.rel8.nas.'+host)
                else :
                    oai.send('echo '+pw+ ' | sudo -S pkill oaisim.rel8.nas.'+host)
                    time.sleep(1)
                    oai.send('echo '+pw+ ' | sudo -S pkill oaisim.rel8.nas.'+host)
        
        oai.rm_driver(oai,user,pw)

    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    try:
        test = '04'
        name = 'Run oai.rel8.phy.rrc'
        diag = 'RRC procedure is not finished completely, check the execution logs and trace BCCH, CCCH, and DCCH channels'
        for i in range(NUM_UE) :
            for j in range(NUM_eNB) :
                conf = '-A AWGN -s 20 -n' + str((i+1+j) * 100) + ' -u' + str(i+1) +' -b'+ str(j+1) + ' -x1'
                trace = logdir + '/log_' + host + case + test + '_' + str(i) + str(j) + '.txt'
                tee = ' 2>&1 | tee ' + trace
                oai.send_expect('./oaisim.rel8.' + host + ' ' + conf + tee, ' Received RRCConnectionReconfigurationComplete from UE ' + str(i),  (i+1) * 200)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '05'
        name = 'Run oai.rel8.phy.rrc.fdd'
        diag = 'RRC procedure is not finished completely in FDD mode, check the execution logs and trace BCCH, CCCH, and DCCH channels'
        for i in range(NUM_UE) :
            for j in range(NUM_eNB) :
                conf = '-A AWGN -F -s 20 -n' + str((i+1+j) * 100) + ' -u' + str(i+1) +' -b'+ str(j+1) + ' -x1'
                trace = logdir + '/log_' + host + case + test + '_' + str(i) + str(j) + '.txt'
                tee = ' 2>&1 | tee ' + trace
                oai.send_expect('./oaisim.rel8.' + host + ' ' + conf + tee, ' Received RRCConnectionReconfigurationComplete from UE ' + str(i),  (i+1) * 200)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '06'
        name = 'Run oai.rel8.itti.abs.rrc'
        diag = 'RRC procedure is not finished completely, check the eNB config file (default is enb.sfr.sud.conf), in addition to the execution logs and trace BCCH, CCCH, and DCCH channels'
        for i in range(NUM_UE) :
            for j in range(NUM_eNB) :
                log_name = logdir + '/log_' + host + case + test + '_' + str(i) + str(j)
                itti_name = log_name + '.log'
                trace_name = log_name + '.txt'
                conf = '-a -l7 -A AWGN --enb-conf ../../PROJECTS/GENERIC-LTE-EPC/CONF/enb.band7.conf -n' + str((i+1+j) * 50) + ' -u' + str(i+1) +' -b'+ str(j+1) + ' -K' + itti_name
                tee = ' 2>&1 | tee -a ' + trace_name
                command = './oaisim.rel8.itti.' + host + ' ' + conf
                oai.send('echo ' + command + ' > ' + trace_name + ';')
                oai.send_expect(command + tee, ' Received RRCConnectionReconfigurationComplete from UE ' + str(i),  (i+1) * 50)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile, trace_name)
    else:
        log.ok(case, test, name, conf, '', logfile)
        

    try:
        test='07'
        name = 'Run oai.rel8.abs.ocg.otg'
        diag = 'Check the scenario if the tests 0202 and 0203 are passed.'
        conf = '-a -c26'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect('./oaisim.rel8.' + host + ' ' + conf + tee, ' DL and UL loss rate below 10 ', 500)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
