#!/bin/sh
echo "----README------"
echo ""
echo "make sure that you have compiled the code with GPROF=1 and installed gprof and graphviz"

echo "set up params"

n_frames=4000
abstraction=1
traffic_load=2
ue="1"
#ping="64 128 256 512 1024 1400"

if [ $abstraction = 1 ]; then 
    option1="-a"
else
    option1="-A AWGN -s 15"
fi;

if [ $traffic_load = 1 ]; then 
    option2="-T1"
fi;
if [ $traffic_load = 2 ]; then 
    option2="-c8"
fi;
if [ $traffic_load = 3 ]; then 
    option2="-T3"
fi;

echo "start oai profiling"
for i in $ue
do
  rm gmon.out gmon.txt
  echo "$OPENAIR_TARGETS/SIMU/USER/oaisim $option1 $option2 -n $n_frames -u $i > /dev/null"
$OPENAIR_TARGETS/SIMU/USER/oaisim $option1 $option2 -n $n_frames -u $i  > /dev/null 
  
  #mv $OPENAIR_TARGETS/SIMU/USER/gmon. .
  gprof $OPENAIR_TARGETS/SIMU/USER/oaisim > gmon.txt
  echo "$OPENAIR_TARGETS/SCRIPTS/gprof2dot.py gmon.txt > profoai.ue$ue.dot"
  $OPENAIR_TARGETS/SCRIPTS/PROFILING/gprof2dot.py gmon.txt > profoai.a$abstraction.n$n_frames.ue$i.dot
  dot -Tpng profoai.a$abstraction.n$n_frames.ue$i.dot > profoai.a$abstraction.n$n_frames.ue$i.png
  echo "oai profiling with $i ue is done"

done

echo "end oai profiling"
