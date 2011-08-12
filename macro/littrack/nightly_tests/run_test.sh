#!/bin/sh

cd /u/andrey/tests/
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

cp /u/andrey/tests/field_v10e.root /u/andrey/tests/nightly/trunk/cbmroot/input/field_v10e.root

cd /u/andrey/tests/nightly/trunk/cbmroot/macro/littrack/nightly_tests/
. ./run_test_electron.sh
. ./run_test_muon.sh

DAY=`date +%y-%m-%d`
cd /u/andrey/web-docs/tests
rm -r -f $DAY
mkdir $DAY

cp -r /u/andrey/tests/nightly/trunk/cbmroot/macro/littrack/nightly_tests/results_electron /u/andrey/web-docs/tests/$DAY/results_electron
cp -r /u/andrey/tests/nightly/trunk/cbmroot/macro/littrack/nightly_tests/results_muon /u/andrey/web-docs/tests/$DAY/results_muon
