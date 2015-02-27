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

# \file case01.py
# \brief test case 01 for OAI: compilations
# \author Navid Nikaein
# \date 2014
# \version 0.1
# @ingroup _test

import log
import openair
import core

makerr1 = '***'
makerr2 = 'Error 1'


def execute(oai, user, pw, host,logfile,logdir,debug):
    
    case = '101'
    rv  = 1; 
    oai.send('cd $OPENAIR1_DIR;')     
    oai.send('cd SIMULATION/LTE_PHY;')   

    try:
        log.start()
        test = '01'
        name = 'Compile oai.rel8.phy.dlsim.make' 
        conf = 'make dlsim'  # PERFECT_CE=1 # for perfect channel estimation
        trace = logdir + '/log_' + case + test + '.txt;'
        tee = ' 2>&1 | tee ' + trace
        diag = 'check the compilation errors for dlsim in $OPENAIR1_DIR/SIMULATION/LTE_PHY'
        oai.send('make clean; make cleanall;')
        oai.send('rm -f ./dlsim.rel8.'+host)
        oai.send_expect_false('make dlsim -j4' + tee, makerr1,  1500)
        oai.send('cp ./dlsim ./dlsim.rel8.'+host)
                   
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
        rv =0
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        log.start()
        test = '02'
        name = 'Compile oai.rel8.phy.ulsim.make' 
        conf = 'make ulsim'
        trace = logdir + '/log_' + case + test + '.txt;'
        tee = ' 2>&1 | tee ' + trace
        diag = 'check the compilation errors for ulsim in $OPENAIR1_DIR/SIMULATION/LTE_PHY'
        oai.send('make cleanall;')
        oai.send('rm -f ./ulsim.rel8.'+host)
        oai.send_expect_false('make ulsim -j4' + tee, makerr1,  1500)
        oai.send('cp ./ulsim ./ulsim.rel8.'+host)
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile,trace)
        rv = 0
    else:
        log.ok(case, test, name, conf, '', logfile)
    
    return rv
