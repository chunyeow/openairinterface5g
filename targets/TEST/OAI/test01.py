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

# \file test01.py
# \brief test 01 for OAI
# \author Navid Nikaein
# \date 2013
# \version 0.1
# @ingroup _test


import sys
import wave
import os
import time
import datetime
import getpass
import math #from time import clock 

import log
import case01
import case02
import case03
import case04
import case05

from  openair import *

debug = 0
prompt2 = '$'
pw =''
i = 0
dlsim=0

for arg in sys.argv:
    if arg == '-d':
        debug = 1
    elif arg == '-dd':
        debug = 2
    elif arg == '-p' :
        prompt2 = sys.argv[i+1]
    elif arg == '-w' :
        pw = sys.argv[i+1]
    elif arg == '-P' :
        dlsim = 1
    elif arg == '-h' :
        print "-d:  low debug level"
        print "-dd: high debug level"
        print "-p:  set the prompt"
        print "-w:  set the password for ssh to localhost"
        sys.exit()
    i= i + 1     

# get the oai object
host = os.uname()[1]
oai = openair('localdomain','localhost')
#start_time = time.time()  # datetime.datetime.now()
try: 
    user = getpass.getuser()
    print '\n******* Note that the user <'+user+'> should be a sudoer *******\n'
    print '******* Connecting to the localhost to perform the test *******\n'
   
   
    if not pw :
        print "username: " + user 
        pw = getpass.getpass() 
    else :
        print "username: " + user 
        #print "password: " + pw 
    
    try:
        prompt = os.getenv("PS1")[-2]
    except : 
        #prompt = input('set your shell prompt: ') 
        prompt = '$'
    print "your prompt is:   " + prompt
    
    oai.connect(user,pw,prompt)
    #oai.get_shell()
except :
    print 'Fail to connect to the local host'
    sys.exit(1)


test = 'test01'
ctime=datetime.datetime.utcnow().strftime("%Y-%m-%d.%Hh%M")
logfile = user+'.'+test+'.'+ctime+'.txt'  
logdir = os.getcwd() + '/pre-ci-logs-'+host;
oai.create_dir(logdir,debug)    
print 'log dir: ' + logdir
#oai.send_nowait('mkdir -p -m 755' + logdir + ';')

#print '=================start the ' + test + ' at ' + ctime + '=================\n'
#print 'Results will be reported in log file : ' + logfile
log.writefile(logfile,'====================start'+test+' at ' + ctime + '=======================\n')
log.set_debug_level(debug)

oai.kill(user, pw)   
oai.rm_driver(oai,user,pw)

# start te test cases 
rv=case01.execute(oai, user, pw, host,logfile,logdir,debug)
if rv != 0 :
    case02.execute(oai, user, pw, host, logfile,logdir,debug)
    case03.execute(oai, user, pw, host, logfile,logdir,debug)
    case04.execute(oai, user, pw, host, logfile,logdir,debug)
    #case05.execute(oai, user, pw, host, logfile,logdir,debug)
else :
    print 'Compilation error: skip test case 02,03,04,05'

oai.kill(user, pw) 
oai.rm_driver(oai,user,pw)

# perform the stats
log.statistics(logfile)


oai.disconnect()

ctime=datetime.datetime.utcnow().strftime("%Y-%m-%d_%Hh%M")
log.writefile(logfile,'====================end the '+ test + ' at ' + ctime +'====================')
print 'Test results can be found in : ' + logfile 
#print '\nThis test took %f minutes\n' % math.ceil((time.time() - start_time)/60) 

#print '\n=====================end the '+ test + ' at ' + ctime + '====================='
