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
    
    case = '04'
    oai.send('cd $OPENAIR1_DIR;')     
    oai.send('cd SIMULATION/LTE_PHY;')   
    
    try:
        test = '00'
        name = 'Perf oai.dlsim.sanity'
        conf = '-a -A AWGN -n 100'
        diag = 'dlsim is not running normally (Segmentation fault / Exiting / FATAL), debugging might be needed'
        trace = logdir + '/log_' + host + case + test + '_1.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./dlsim.rel8.' + host + ' ' + conf + tee, 'Segmentation fault', 30)
        trace = logdir + '/log_' + host + case + test + '_2.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./dlsim.rel8.' + host + ' ' + conf + tee, 'Exiting', 30)
        trace = logdir + '/log_' + host + case + test + '_3.txt;'
        tee = ' 2>&1 | tee ' + trace
        oai.send_expect_false('./dlsim.rel8.' + host + ' ' + conf + tee, 'FATAL', 30)

    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
    
    try:
        test = '01'
        name = 'Perf oai.dlsim.test1'
        diag = 'Test 1, 10 MHz, R2.FDD (MCS 5), EVA5, -1dB'
        conf = '-m5 -gF -s-1 -w1.0 -f.2 -n500 -B50 -c2 -z2 -O70'
        trace = logdir + '/log_' + host + case + test +'.txt'
        tee = ' 2>&1 | tee ' + trace
        cmd = 'taskset -c 0 ./dlsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    try:
        test = '06'
        name = 'Perf oai.dlsim.test5'
        diag = 'Test 5, 1.4 MHz, R4.FDD (MCS 4), EVA5, 0dB (70%)'
        conf = '-m4 -gF -s0 -w1.0 -f.2 -n500 -B6 -c4 -z2 -O70'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        cmd = 'taskset -c 0 ./dlsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, ' effective rate passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
  
    try:
        test = '06'
        name = 'Perf oai.dlsim.test6'
        diag = 'Test 6, 10 MHz, R3.FDD (MCS 15), EVA5, 6.7dB (70%)'
        conf = '-m15 -gF -s6.7 -w1.0 -f.2 -n500 -B50 -c2 -z2 -O70'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        cmd = 'taskset -c 0 ./dlsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
  
    try:
        test = '06b'
        name = 'Perf oai.dlsim.test6b'
        diag = 'Test 6b, 5 MHz, R3-1.FDD (MCS 15), EVA5, 6.7dB (70%)'
        conf = '-m14 -gF -s6.7 -w1.0 -f.2 -n500 -B25 -c3 -z2 -O70'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        cmd = 'taskset -c 0 ./dlsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)
  
    try:
        test = '07'
        name = 'Perf oai.dlsim.test7'
        diag = 'Test 6b, 5 MHz, R3-1.FDD (MCS 15), EVA5, 6.7dB (30%)'
        conf = '-m15 -gG -s6.7 -w1.0 -f.2 -n500 -B50 -c2 -z2 -O30'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        cmd = 'taskset -c 0 ./dlsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)


    try:
        test = '07b'
        name = 'Perf oai.dlsim.test7b'
        diag = 'Test 7b, 5 MHz, R3-1.FDD (MCS 15), ETU70, 1.4 dB (30%)'
        conf = '-m14 -gG -s1.4 -w1.0 -f.2 -n500 -B25 -c3 -z2 -O30'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        cmd = 'taskset -c 0 ./dlsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '10'
        name = 'Perf oai.dlsim.test10'
        diag = 'Test 10, 5 MHz, R6.FDD (MCS 25), EVA5, 17.4 dB (70%)'
        conf = '-m25 -gF -s17.4 -w1.0 -f.2 -n500 -B25 -c3 -z2 -O70'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        cmd = 'taskset -c 0 ./dlsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '10b'
        name = 'Perf oai.dlsim.test10b'
        diag = 'Test 10b, 5 MHz, R6-1.FDD (MCS 24,18 PRB), EVA5, 17.5dB (70%)'
        conf = '-m25 -gF -s17.5 -w1.0 -f.2 -n500 -B25 -c3 -z2 -r1022 -O70'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        cmd = 'taskset -c 0 ./dlsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '11'
        name = 'Perf oai.dlsim.test11'
        diag = 'Test 11, 10 MHz, R7.FDD (MCS 25), EVA5, 17.7dB (70%)'
        conf = '-m26 -gF -s17.7 -w1.0 -f.2 -n500 -B50 -c2 -z2 -O70'
        trace = logdir + '/log_' + host + case + test + '.txt'
        tee = ' 2>&1 | tee ' + trace
        cmd = 'taskset -c 0 ./dlsim.rel8.' + host + ' ' + conf + tee
        oai.send_expect(cmd, 'passed', 150)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
    else:
        log.ok(case, test, name, conf, '', logfile)


