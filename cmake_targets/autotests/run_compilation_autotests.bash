#!/bin/bash

if [ -s $OPENAIR_DIR/cmake_targets/build_helper.bash ] ; then
   source $OPENAIR_DIR/cmake_targets/build_helper.bash
else
   echo "Error: no file in the file tree: is OPENAIR_DIR variable set?"
   exit 1
fi

tdir=$OPENAIR_DIR/cmake_targets/autotests
mkdir -p $tdir/bin $tdir/log

updated=$(svn st -q $OPENAIR_DIR)
if [ "$updated" != "" ] ; then
	echo_warning "some files are not in svn: $updated"
fi

compilations \
    autotests/test.0101 oaisim \
    oaisim  $tdir/bin/oaisim.r8 \
    $tdir/log/test0101.txt \
	"test 0101:oaisim Rel8 passed" \
    "test 0101:oaisim Rel8 failed"

compilations \
    autotests/test.0102 oaisim \
    oaisim  $tdir/bin/oaisim.r8.nas \
    $tdir/log/test0102.oaisim.txt \
	"test 0102:oaisim Rel8 nas passed" \
    "test 0102:oaisim Rel8 nas failed"
compilations \
    autotests/test.0103 oaisim \
    oaisim  $tdir/bin/oaisim.r8.rf \
    $tdir/log/test0103.txt \
	"test 0103:oaisim rel8 rf passed" \
    "test 0103:oaisim rel8 rf failed"
compilations \
    autotests/test.0104 dlsim \
    dlsim  $tdir/bin/dlsim \
    $tdir/log/test0104.txt \
	"test 0104:dlsim passed" \
    "test 0104:dlsim failed"    
compilations \
    autotests/test.0104 ulsim \
    ulsim  $tdir/bin/ulsim \
    $tdir/log/test0105.txt \
	"test 0105: ulsim passed" \
    "test 0105: ulsim failed"
compilations \
    autotests/test.0106 oaisim \
    oaisim  $tdir/bin/oaisim.r8.itti \
    $tdir/log/test0106.txt \
	"test 0103:oaisim rel8 itti passed" \
    "test 0103:oaisim rel8 itti failed"
compilations \
    autotests/test.0107 oaisim \
    oaisim  $tdir/bin/oaisim.r10 \
    $tdir/log/test0107.txt \
	"test 0103:oaisim rel10 passed" \
    "test 0103:oaisim rel10 failed"
compilations \
    autotests/test.0108 oaisim \
    oaisim  $tdir/bin/oaisim.r10.itti \
   $tdir/log/test0108.txt \
	"test 0108:oaisim rel10 itti passed" \
    "test 0108:oaisim rel10 itti failed"
compilations \
    autotests/test.0114 oaisim \
    oaisim  $tdir/bin/oaisim.r8.itti.ral \
    $tdir/log/test0114.txt \
	"test 0114:oaisim rel8 itti ral passed" \
    "test 0114:oaisim rel8 itti ral failed"
compilations \
    autotests/test.0115 oaisim \
    oaisim  $tdir/bin/oaisim.r10.itti.ral \
    $tdir/log/test0115.txt \
	"test 0114:oaisim rel10 itti ral passed" \
    "test 0114:oaisim rel10 itti ral failed" 
compilations \
    autotests/test.0102 nasmesh \
    CMakeFiles/nasmesh/nasmesh.ko $tdir/bin/nasmesh.ko \
    $tdir/log/test0120.txt \
	"test 0120: nasmesh.ko passed" \
    "test 0120: nasmesk.ko failed"

