#!/bin/sh
cd $MY_BUILD_DIR
. ./config.sh
cd -

root -b -q "./run_sim_geotest.C($NEVENTS)"

root  -l "./run_reco_geotest.C($NEVENTS)"
