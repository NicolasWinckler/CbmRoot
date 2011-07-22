#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -

#root -b -q "./run_sim.C($NEVENTS)"
 
#root -b -q "./run_reco.C($NEVENTS)"

#root -b -q "./global_reco_ideal.C($NEVENTS)"

#root -b -q "./run_reco_rich.C($NEVENTS)"

for K in 1 ; do
	export DILEPANALYSISFILE=$DIR/mytask.analysis.nodelta.all.ideal.$XXXX.root
	root -b -q "../../analysis/dielectron/run_analysis.C($NEVENTS,$K)"
done

#root -b -q "../../littrack/global_reco_qa.C($NEVENTS)"