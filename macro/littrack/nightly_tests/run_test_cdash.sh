#!/bin/sh

. $VMCWORKDIR/Dart.sh Nightly Dart.cfg    

DAY=`date +%y-%m-%d`
cd /u/andrey/web-docs/tests
rm -r -f $DAY
mkdir $DAY

cp -r /data.local1/andrey/cbm_nightly/macro/littrack/nightly_tests/results_electron /u/andrey/web-docs/tests/$DAY/results_electron
cp -r /data.local1/andrey/cbm_nightly/macro/littrack/nightly_tests/results_muon /u/andrey/web-docs/tests/$DAY/results_muon
