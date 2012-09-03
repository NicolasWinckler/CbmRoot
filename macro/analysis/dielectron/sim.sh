#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -

root -b -q "./run_sim.C($NEVENTS)"
 
root -b -q "./run_reco.C($NEVENTS)"

root -b -q "./run_analysis.C($NEVENTS)"
