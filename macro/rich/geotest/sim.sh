#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -

root -b -q "./run_sim_geotest.C($NEVENTS)"

root -b -q "./run_reco_geotest.C($NEVENTS)"
