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

def execute(oai, user, pw, host,logfile,logdir,debug):
    
    case = '04'
    oai.send('cd $OPENAIR1_DIR;')     
    oai.send('cd SIMULATION/LTE_PHY;')   
    
    try:
        log.start()
        test = '00'
        name = 'Perf oai.dlsim.sanity'
        conf = '-a -A AWGN -n 100'
        diag = 'ulsim is not running normally (Segmentation fault / Exiting / FATAL), debugging might be needed'
        trace = logdir + '/log_' + host + case + test + '_1.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./ulsim.rel8.' + host + ' ' + conf + tee, 'Segmentation fault', 30)
        trace = logdir + '/log_' + host + case + test + '_2.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./ulsim.rel8.' + host + ' ' + conf + tee, 'Exiting', 30)
        trace = logdir + '/log_' + host + case + test + '_3.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./ulsim.rel8.' + host + ' ' + conf + tee, 'FATAL', 30)

    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
    
    try:
        log.start()
        test = '01'
        name = 'Perf oai.ulsim.test1'
        diag = 'Test 1, 5 MHz, FDD (MCS 5), AWGN, 6dB'
        conf = '-B25 -m5 -y1 -gN -x1 -s6 -w1.0 -e.1 -P -n500 -O70 -L'
        trace = logdir + '/log_' + host + case + test +'.txt'
        tee = ' 2>&1 | tee ' + trace
        #print test + 'not performed'
        cmd = 'taskset -c 0 ./ulsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    try:
        log.start()
        test = '02'
        name = 'Perf oai.ulsim.test2'
        diag = 'Test 2, 5 MHz, FDD (MCS 16), AWGN , 12dB (70%)'
        conf = '-B25 -m16 -y1 -gN -x1 -s12 -w1.0 -e.1 -P -n500 -O70 -L'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        #print test + 'not performed'
        cmd = 'taskset -c 0 ./ulsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
  
    try:
        log.start()
        test = '03'
        name = 'Perf oai.ulsim.test3'
        diag = 'Test 3, 10 MHz, R3.FDD (MCS 5), AWGN, 6dB (70%)'
        conf = '-B50 -m5 -y1 -gN -x1 -s6 -w1.0 -e.1 -P -n500 -O70 -L'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        #print test + 'not performed'
        cmd = 'taskset -c 0 ./ulsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
  
    try:
        log.start()
        test = '04'
        name = 'Perf oai.ulsim.test4'
        diag = 'Test 4, 10 MHz, R3-1.FDD (MCS 16), AWGN, 12dB (70%)'
        conf = '-B50 -m16 -y1 -gN -x1 -s12 -w1.0 -e.1 -P -n500 -O70 -L'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        #print test + 'not performed'
        cmd = 'taskset -c 0 ./ulsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
  
    try:
        log.start()
        test = '05'
        name = 'Perf oai.ulsim.test7'
        diag = 'Test 5, 20 MHz, FDD (MCS 5), AWGN, 6dB (70%)'
        conf = '-B100 -m5 -y1 -gN -x1 -s6 -w1.0 -e.1 -P -n500 -O70 -L'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        #print test + 'not performed'
        cmd = 'taskset -c 0 ./ulsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        log.start()
        test = '06'
        name = 'Perf oai.ulsim.test10'
        diag = 'Test 06, 20 MHz, FDD (MCS 16), AWGN, 12 dB (70%)'
        conf = '-B100 -m16 -y1 -gN -x1 -s12 -w1.0 -e.1 -P -n500 -O70 -L'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        #print test + 'not performed'
        cmd = 'taskset -c 0 ./ulsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)

 

