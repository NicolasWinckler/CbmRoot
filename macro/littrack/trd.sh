#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -

root -b -q ./trd_sim.C($NEVENTS)

root -b -q ./sts_reco.C($NEVENTS)

root -b -q ./trd_hits.C($NEVENTS)

root -b -q ./trd_reco.C($NEVENTS)

