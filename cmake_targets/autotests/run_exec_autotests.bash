#!/bin/bash

if [ -s $OPENAIR_DIR/cmake_targets/tools/build_helper.bash ] ; then
   source $OPENAIR_DIR/cmake_targets/tools/build_helper.bash
else
   echo "Error: no file in the file tree: is OPENAIR_DIR variable set?"
   exit 1
fi

dbin=$OPENAIR_DIR/cmake_targets/autotests/bin
dlog=$OPENAIR_DIR/cmake_targets/autotests/log

run_test() {
case=case$1; shift
cmd=$1; shift
expected=$3; shift
$cmd > $dlog/$case.txt 2>&1
if [ $expected = "true" ] ; then	 
  if $* $dlog/$case.txt; then
    echo_success "test $case, command: $cmd ok"
  else
    echo_error "test $case, command: $cmd Failed"
  fi
else 
  if $* $dlog/$case.txt; then
    echo_error "test $case, command: $cmd Failed"
  else
    echo_success "test $case, command: $cmd ok"
  fi
fi
}

run_test 0200 "$dbin/oaisim.r8 -a -A AWGN -n 100" false grep -q '(Segmentation.fault)|(Exiting)|(FATAL)'

run_test 0201 "$dbin/oaisim.r8 -a -A AWGN -n 100" false fgrep -q '[E]'

