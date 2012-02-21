#!/bin/sh

# Pathes and parameters
TEST_DIR=/data.local1/andrey/tests/cdash_tests
BUILD_DIR=${TEST_DIR}/cbmroot_build_jan12

function checkout_cbmroot
{
    dir=$1
    rm -rf ${dir}/*
    cd ${dir}/
    svn co  https://subversion.gsi.de/fairroot/cbmroot/trunk cbmroot
    cp -R /data.local1/andrey/tests/fieldmaps/* ${dir}/cbmroot/input/
    cp /data.local1/andrey/tests/cdash_config/Dart.cfg ${dir}/cbmroot/Dart.cfg
}

function build_cbmroot 
{
    src_dir=$1
    build_dir=$2
    simpath=$3
    export SIMPATH=${simpath} #/data.local1/fairsoft/fairsoft_jan12/install
    mkdir ${build_dir}
    cd ${build_dir}
    cmake ${src_dir}
    . ${build_dir}/config.sh
    make -j8
}

function copy_results_to_web
{
    src_dir=$1
    web_dir=/u/andrey/web-docs/tests
    day=`date +%y-%m-%d`
    rm -rf ${web_dir}/${day}
    mkdir ${web_dir}/${day}
    cp -R ${src_dir} ${web_dir}/${day}/
    chmod -R u+rwX,g+rx,o+rx ${web_dir}/${DAY}/
}

# Run test without CDash
if [ "$1" = "build" ] ; then
    checkout_cbmroot ${TEST_DIR}
    build_cbmroot ${TEST_DIR}/cbmroot ${BUILD_DIR} /data.local1/fairsoft/fairsoft_jan12/install

    # Run tests
    . ${VMCWORKDIR}/macro/littrack/nightly_tests/run_test_electron.sh
    . ${VMCWORKDIR}/macro/littrack/nightly_tests/run_test_muon.sh
    
    copy_results_to_web "${TEST_DIR}/cbmroot/macro/littrack/nightly_tests/std_reco*"
elif [ "$1" = "cdash" ] ; then 
    checkout_cbmroot ${TEST_DIR}
    # Run tests with CDash
    . ${TEST_DIR}/cbmroot/Dart.sh Experimental ${TEST_DIR}/cbmroot/Dart.cfg
    
    copy_results_to_web "${BUILD_DIR}/macro/littrack/nightly_tests/std_reco*"
elif [ "$1" = "local" ] ; then
    . ./run_test_electron.sh
    . ./run_test_muon.sh
    
    copy_results_to_web "./std_reco*"
else
    echo 'Error! Option $1 does not supported!'
    exit 2
fi
