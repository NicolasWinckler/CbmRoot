#!/bin/sh

# Pathes and parameters
TEST_DIR=/data.local1/andrey/cdash_tests/test
EXTERNALS=jan12
BUILD_DIR=$TEST_DIR/cbmroot_build_$EXTERNALS

# Checkout and configure CBMROOT
rm -rf $TEST_DIR/*
export SIMPATH=/data.local1/fairsoft/fairsoft_$EXTERNALS/
svn co  https://subversion.gsi.de/fairroot/cbmroot/trunk cbmroot
cp /data.local1/andrey/tests/field_v10e.root $TEST_DIR/cbmroot/input/field_v10e.root
cp /data.local1/andrey/cdash_tests/Dart.cfg $TEST_DIR/cbmroot/Dart.cfg

. $TEST_DIR/cbmroot/Dart.sh Experimental $TEST_DIR/cbmroot/Dart.cfg    

DAY=`date +%y-%m-%d`
cd /u/andrey/web-docs/tests
rm -r -f $DAY
mkdir $DAY

cp -r $BUILD_DIR/macro/littrack/nightly_tests/results_electron_branch /u/andrey/web-docs/tests/$DAY/results_electron_branch
cp -r $BUILD_DIR/macro/littrack/nightly_tests/results_electron_nn /u/andrey/web-docs/tests/$DAY/results_electron_nn
cp -r $BUILD_DIR/macro/littrack/nightly_tests/results_electron_nn_parallel /u/andrey/web-docs/tests/$DAY/results_electron_nn_parallel

cp -r $BUILD_DIR/macro/littrack/nightly_tests/results_muon_branch /u/andrey/web-docs/tests/$DAY/results_muon_branch
cp -r $BUILD_DIR/macro/littrack/nightly_tests/results_muon_nn /u/andrey/web-docs/tests/$DAY/results_muon_nn
cp -r $BUILD_DIR/macro/littrack/nightly_tests/results_muon_nn_parallel /u/andrey/web-docs/tests/$DAY/results_muon_nn_parallel

export SCRIPT=yes

function study_summary()
{
    cd /u/andrey/web-docs/tests/$DAY/
    rm -r -f summary_$1
    mkdir summary_$1
    cd summary_$1
    export SCRIPT=yes
    export STUDYOUTPUTDIR=./
    export NSTUDIES=3
    export STUDYRESULT1=../results_$1_branch/
    export STUDYNAME1=branch
    export STUDYRESULT2=../results_$1_nn/
    export STUDYNAME2=nn
    export STUDYRESULT3=../results_$1_nn_parallel/
    export STUDYNAME3=nn_parallel
    root -b -q -l "$VMCWORKDIR/macro/littrack/qa_study.C"
}

study_summary muon
study_summary electron

chmod -R u+rwX,g+rx,o+rx /u/andrey/web-docs/tests/$DAY/

export SCRIPT=no
