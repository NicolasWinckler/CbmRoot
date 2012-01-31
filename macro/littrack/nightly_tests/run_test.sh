#!/bin/sh

cd /data.local1/andrey/tests/
rm -r -f nightly
mkdir nightly
cd nightly
mkdir trunk
cd trunk

export SIMPATH=/data.local1/fairsoft/fairsoft_may11/
svn co  https://subversion.gsi.de/fairroot/cbmroot/trunk cbmroot
mkdir build_lxir039
cd build_lxir039
cmake ../cbmroot
. ./config.sh
make -j6

cp /data.local1/andrey/tests/field_v10e.root /data.local1/andrey/tests/nightly/trunk/cbmroot/input/field_v10e.root

cd /data.local1/andrey/tests/nightly/trunk/cbmroot/macro/littrack/nightly_tests/
. ./run_test_electron.sh
. ./run_test_muon.sh

export SCRIPT=yes

function study_summary()
{
    cd /data.local1/andrey/tests/nightly/trunk/cbmroot/macro/littrack/nightly_tests/
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

export SCRIPT=no
