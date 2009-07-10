#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -

root -b -q "./much_sim.C($NEVENTS)"

root -b -q "./global_hits.C($NEVENTS)"

root -b -q "./global_tracking.C($NEVENTS)"

#root -b -q "./global_reco.C($NEVENTS)"
