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
# \brief test case 01 for OAI
# \author Navid Nikaein
# \date 2013
# \version 0.1
# @ingroup _test

import log
import openair
import core

makerr1 = 'make: ***'
makerr2 = 'Error 1'

def execute(oai, user, pw, logfile):
    
    case = '01'
    oai.send('cd $OPENAIR_TARGETS;')   
  
    try:
        test = '00'
        name = 'Check oai.svn.add'
        conf = 'svn st -q | grep makefile'
        diag = 'Makefile(s) changed. If you are adding a new file, make sure that it is added to the svn'
        rsp = oai.send_recv('svn st -q | grep -i makefile;') 
        for item in rsp.split("\n"):
            if "Makefile" in item:
                rsp2=item.strip() + '\n'
        oai.find_false_re(rsp,'Makefile')
    except log.err, e:
        diag = diag + "\n" + rsp2  
               #log.skip(case, test, name, conf, e.value, logfile)
        log.skip(case, test, name, conf, '', diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)
    
    oai.send('cd SIMU/USER;')   
  
    try:
        test = '01'
        name = 'Compile oai.rel8.make'
        conf = 'make'
        diag = "check the compilation errors for oai"
        oai.send('make cleanall;')
        oai.send('make cleanasn1;')
        oai.send('rm -f ./oaisim.rel8;')
        oai.send_expect_false('make -j4;', makerr1,  1500)
        oai.send('cp ./oaisim ./oaisim.rel8;')
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    try:
        test = '02'
        name = 'Compile oai.rel8.nas.make'
        conf = 'make nasmesh_fix; make NAS=1'
        diag = 'check the compilation errors for oai and nas driver'
        oai.send('make cleanall;')
        oai.send('rm -f ./oaisim.rel8.nas;')
        oai.send('rm -f ./nasmesh;')
        oai.send_expect_false('make NAS=1 -j4;', makerr1,  1500)
        oai.send('cp ./oaisim ./oaisim.rel8.nas;')
        if user == 'root' : 
            oai.send_nowait('rmmod nasmesh;')
            oai.send_expect_false('make nasmesh_fix;', makerr1,  60)
        else :
            oai.send_nowait('echo '+pw+ ' | sudo -S rmmod nasmesh;')
            oai.send_expect_false('make test_nasmesh_fix;', makerr1,  60)
            oai.send_nowait('echo '+pw+ ' | sudo -S insmod ./nasmesh.ko;')
        
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)
    
    oai.send('cd $OPENAIR_TARGETS;')     
    oai.send('cd RTAI/USER;')   

    try:
        test = '03'
        name = 'Compile oai.rel8.rf.make' 
        conf = 'make RTAI=0 Rel8=1'
        diag = 'check the compilation errors for Rel8'
        oai.send('make cleanall;')
        oai.send('rm -f ./oaisim.rel8.rf;')
        oai.send_expect_false('make RTAI=0 -j4;', makerr1,  1500)
        oai.send('cp ./oaisim ./oaisim.rel8.rf;')
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)
        
    oai.send('cd $OPENAIR1_DIR;')     
    oai.send('cd SIMULATION/LTE_PHY;')   

    try:
        test = '04'
        name = 'Compile oai.rel8.phy.dlsim.make' 
        conf = 'make dlsim'
        diag = 'check the compilation errors for dlsim in $OPENAIR1_DIR/SIMULATION/LTE_PHY'
        oai.send('make clean;')
        oai.send('rm -f ./dlsim.rel8;')
        oai.send_expect_false('make dlsim -j4;', makerr1,  1500)
        oai.send('cp ./dlsim ./dlsim.rel8;')
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '05'
        name = 'Compile oai.rel8.phy.ulsim.make' 
        conf = 'make ulsim'
        diag = 'check the compilation errors for dlsim in $OPENAIR1_DIR/SIMULATION/LTE_PHY'
        oai.send('make clean;')
        oai.send('rm -f ./ulsim.rel8;')
        oai.send_expect_false('make ulsim -j4;', makerr1,  1500)
        oai.send('cp ./ulsim ./ulsim.rel8;')
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)
    
    oai.send('cd $OPENAIR_TARGETS;')     
    oai.send('cd SIMU/USER;')   
    
    try:
        test = '06'
        name = 'Compile oai.rel10.make' 
        conf = 'make Rel10=1'
        diag = 'check the compilation errors for Rel10'
        oai.send('make clean;')
        oai.send('make cleanall;')
        oai.send('make cleanasn1;')
        oai.send('rm -f ./oaisim.rel10;')
        oai.send_expect_false('make Rel10=1 -j4;', makerr1,  1500)
        oai.send('cp ./oaisim ./oaisim.rel10;')
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '07'
        name = 'Compile oai.rel8.cellular.rg.make'
        conf = 'make rrc_cellular=1 eNB_flag=1'
        diag = 'check the compilation errors for RRC Cellular (node RG)'
        oai.send('make clean;')
        oai.send('make cleanall;')
        oai.send('make cleanasn1;')
        oai.send('rm -f ./oaisim.rel8.cellular.rg;')
        oai.send_expect_false('make rrc_cellular=1 eNB_flag=1 -j4;', makerr1,  1500)
        oai.send('cp ./oaisim /oaisim.rel8.cellular.rg;')
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)

    try:
        test = '08'
        name = 'Compile oai.rel8.cellular.mt.make'
        conf = 'make rrc_cellular=1 UE_flag=1'
        diag = 'check the compilation errors for RRC Cellular (node MT)'
        oai.send('make clean;')
        oai.send('make cleanall;')
        oai.send('make cleanasn1;')
        oai.send('rm -f ./oaisim.rel8.cellular.mt;')
        oai.send_expect_false('make rrc_cellular=1 UE_flag=1 -j4;', makerr1,  1500)
        oai.send('cp ./oaisim /oaisim.rel8.cellular.mt;')
    except log.err, e:
        log.fail(case, test, name, conf, e.value, diag, logfile)
    else:
        log.ok(case, test, name, conf, '', logfile)

    #try:
        #test = '09'
        #name = 'Compile nas_sim_rg_cellular'
        #conf = 'make nas_sim_rg_cellular'
        #diag = 'check the compilation errors for NAS SIM RRC Cellular (node RG)'
        #oai.send('make clean;')
        #oai.send('make cleanall;')
        #oai.send('make cleanasn1;')
        #oai.send('make cleancell')
        #oai.send('rm -f ./nas_sim_rg_cellular;')
        #oai.send_expect_false('make nas_sim_rg_cellular -j4;', makerr1,  1500)
    #except log.err, e:
        #log.fail(case, test, name, conf, e.value, diag, logfile)
    #else:
        #log.ok(case, test, name, conf, '', logfile)

    #try:
        #test = '10'
        #name = 'Compile nas_sim_mt_cellular'
        #conf = 'make nas_sim_mt_cellular'
        #diag = 'check the compilation errors for NAS SIM RRC Cellular (node MT)'
        #oai.send('make clean;')
        #oai.send('make cleanall;')
        #oai.send('make cleanasn1;')
        #oai.send('make cleancell')
        #oai.send('rm -f ./nas_sim_mt_cellular;')
        #oai.send_expect_false('make nas_sim_mt_cellular -j4;', makerr1,  1500)
    #except log.err, e:
        #log.fail(case, test, name, conf, e.value, diag, logfile)
    #else:
        #log.ok(case, test, name, conf, '', logfile)

