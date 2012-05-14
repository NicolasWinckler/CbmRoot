#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -

root -b -q "./run_sim_geotest.C($NEVENTS)"

root  -l "./run_reco_geotest.C($NEVENTS)"
