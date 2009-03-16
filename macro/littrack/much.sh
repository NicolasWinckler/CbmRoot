#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -

root -b -q ./much_sim.C($NEVENTS)

root -b -q ./sts_reco.C($NEVENTS)

root -b -q ./much_hits.C($NEVENTS)

root -b -q ./much_reco.C($NEVENTS)

