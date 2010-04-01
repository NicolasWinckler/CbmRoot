#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -

root -b -q "./global_sim.C($NEVENTS)"

root -b -q "./global_hits.C($NEVENTS)"

root -b -q "./global_tracking.C($NEVENTS)"

#root -b -q "./global_reco.C($NEVENTS)"


#FOR TRD TEST

#for X in 100 200 300 400 500 600 700 800 900 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000 30000 50000 70000 100000; do
#for X in 1000; do
#	export TRDHITERR=$X
#	export GLOBALHITSFILE=$DIR/global.hits.trd$TRDHITERR.0000.root
#	export GLOBALTRACKSFILE=$DIR/global.tracks.trd$TRDHITERR.0000.root
#	export GLOBALTRACKSIDEALFILE=$DIR/global.tracks.ideal.trd$TRDHITERR.0000.root
#	export PROPANAFILE=$DIR/propagation.ana.trd$TRDHITERR.0000.root
#	export IMAGEDIR=./test/trdstd_urqmd/trd$TRDHITERR/
#	rm -r $IMAGEDIR
#	mkdir $IMAGEDIR
#	root -b -q "./global_hits.C($NEVENTS)"
#	root -b -q "./global_tracking.C($NEVENTS)"
#	root -b -q "./global_reco_ideal.C($NEVENTS)"
#	root -b -q "./prop_ana.C($NEVENTS)"
#done

