#!/usr/bin/env python
import subprocess
import optparse
import re 

OPT=" "
TERMINAL="gnome-terminal -x "
TIME="time"

def runBash(cmd):
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    out = p.stdout.read().strip()
    return out 
    
def main():
    global OPT,TIME,TERMINAL
    subprocess.call('ps | nawk \'/oaisim/ {system("kill " $1)}\'', shell=True)
    p = optparse.OptionParser()
    p.add_option('--eNB', '-b', default="1")
    p.add_option('--UE', '-u', default="1")
    p.add_option('--frame','-n', default="100")
    p.add_option('--terminal','-t', default="1")
    p.add_option('--time','--time', default="0")
    options, arguments = p.parse_args()
    print '---- Configuration ---- '
    print 'Number of eNB : %s' % options.eNB
    print 'Number of UE  : %s' % options.UE
    if(options.terminal=='1'):
       OPT+=TERMINAL
    if(options.time=='1'):
        OPT+=TIME
    
    #CHANNEL= 'valgrind --tool=callgrind --collect-bus=yes --branch-sim=yes $OPENAIR_TARGETS/SIMU/USER/oaisim -X0 -b%s -u%s -n%s' % (options.eNB,options.UE,options.frame)
    CHANNEL= '$OPENAIR_TARGETS/SIMU/USER/oaisim -X0 -b%s -u%s -n%s' % (options.eNB,options.UE,options.frame)
    #subprocess.call(CHANNEL, shell=True)
    print CHANNEL
    
    nb_enb=int(options.eNB)
    nb_ue=int(options.UE)
    frame=int(options.frame)
  
    for x in xrange(0,nb_enb):
        ENB='%s $OPENAIR_TARGETS/SIMU/USER/oaisim -X1 -i%s  &' % (OPT,x)
        print ENB
        subprocess.call(ENB,shell=True)
        subprocess.call('sleep 1', shell=True)
        
    for x in xrange(0,nb_ue):
        UE='%s $OPENAIR_TARGETS/SIMU/USER/oaisim -X2 -i%s &' % (OPT,x)
        print UE
        subprocess.call(UE,shell=True)
        subprocess.call('sleep 1', shell=True)
        
   # subprocess.call('sleep 2', shell=True)
    subprocess.call(CHANNEL, shell=True)
   # subprocess.call('ps | nawk \'/oaisim/ {system("kill " $1)}\'', shell=True)
    
if __name__ == '__main__':
    main()
    

