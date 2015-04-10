#!/bin/bash

if [ -s $OPENAIR_DIR/cmake_targets/tools/build_helper ] ; then
   source $OPENAIR_DIR/cmake_targets/tools/build_helper
else
   echo "Error: no file in the file tree: is OPENAIR_DIR variable set?"
   exit 1
fi

test_compile() {
    mkdir -p $tdir/$1/build
    cd $tdir/$1/build
    {
	cmake ..
	rm -f $3
	make -j4 $2
    } > $tdir/log/$1.txt 2>&1
    if [ -s $3 ] ; then
     cp $3 $tdir/bin/`basename $3`.$1
     echo_success "$1 test compiled"
  else
     echo_error "$1 test compilation failed"
  fi
}

tdir=$OPENAIR_DIR/cmake_targets/autotests
mkdir -p $tdir/bin $tdir/log

updated=$(svn st -q $OPENAIR_DIR)
if [ "$updated" != "" ] ; then
	echo_warning "some files are not in svn: $updated"
fi

cd $tdir 

test_compile \
    test.0101 oaisim \
    oaisim  $tdir/bin/oaisim.r8

test_compile \
    test.0102 oaisim \
    oaisim  $tdir/bin/oaisim.r8.nas

test_compile \
    test.0103 lte-softmodem \
    lte-softmodem  $tdir/bin/lte-softmodem.r8.rf

test_compile \
    test.0104 dlsim \
    dlsim  $tdir/bin/dlsim

test_compile \
    test.0104 ulsim \
    ulsim  $tdir/bin/ulsim

test_compile \
    test.0106 oaisim \
    oaisim  $tdir/bin/oaisim.r8.itti

test_compile \
    test.0107 oaisim \
    oaisim  $tdir/bin/oaisim.r10

test_compile \
    test.0108 oaisim \
    oaisim  $tdir/bin/oaisim.r10.itti

test_compile \
    test.0114 oaisim \
    oaisim  $tdir/bin/oaisim.r8.itti.ral

test_compile \
    test.0115 oaisim \
    oaisim  $tdir/bin/oaisim.r10.itti.ral

test_compile \
    test.0120 nasmesh \
    CMakeFiles/nasmesh/nasmesh.ko $tdir/bin/nasmesh.ko

