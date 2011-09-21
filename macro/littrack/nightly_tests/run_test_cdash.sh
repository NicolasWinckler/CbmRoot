#!/bin/sh

. /data.local1/andrey/cbm_nightly/Dart.sh Nightly /data.local1/andrey/cbm_nightly/Dart.cfg    

. /tmp/andrey/build_cbm_may11/config.sh

#. ./run_test_electron.sh
#. ./run_test_muon.sh

DAY=`date +%y-%m-%d`
cd /u/andrey/web-docs/tests
rm -r -f $DAY
mkdir $DAY

cp -r /tmp/andrey/build_cbm_may11/macro/littrack/nightly_tests/results_electron_branch /u/andrey/web-docs/tests/$DAY/results_electron_branch
cp -r /tmp/andrey/build_cbm_may11/macro/littrack/nightly_tests/results_electron_nn /u/andrey/web-docs/tests/$DAY/results_electron_nn
cp -r /tmp/andrey/build_cbm_may11/macro/littrack/nightly_tests/results_electron_nn_parallel /u/andrey/web-docs/tests/$DAY/results_electron_nn_parallel

cp -r /tmp/andrey/build_cbm_may11/macro/littrack/nightly_tests/results_muon_branch /u/andrey/web-docs/tests/$DAY/results_muon_branch
cp -r /tmp/andrey/build_cbm_may11/macro/littrack/nightly_tests/results_muon_nn /u/andrey/web-docs/tests/$DAY/results_muon_nn
cp -r /tmp/andrey/build_cbm_may11/macro/littrack/nightly_tests/results_muon_nn_parallel /u/andrey/web-docs/tests/$DAY/results_muon_branch

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
    export STUDYRESULT1=../results_$1_branch
    export STUDYNAME1=branch
    export STUDYRESULT2=../results_$1_nn
    export STUDYNAME2=nn
    export STUDYRESULT3=../results_$1_nn_parallel
    export STUDYNAME3=nn_parallel
 
    if [ "$1" = "muon" ] ; then    
        export STUDYELECTRONSETUP=0
        export STUDYMVD=0
        export STUDYSTS=1
        export STUDYRICH=0
        export STUDYTRD=0
        export STUDYMUCH=1
        export STUDYTOF=1
    else
    if [ "$1" = "electron" ] ; then
        export STUDYELECTRONSETUP=1
        export STUDYMVD=0
        export STUDYSTS=1
        export STUDYRICH=1
        export STUDYTRD=1
        export STUDYMUCH=0
        export STUDYTOF=1
    fi
    fi

    root -b -q -l "$VMCWORKDIR/macro/littrack/qa_study.C"
}

study_summary muon
study_summary electron

chmod -R u+rwX,g+rx,o+rx /u/andrey/web-docs/tests/$DAY/

export SCRIPT=no


