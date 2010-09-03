#!/bin/sh

# Import common functionality for scripts
. ./common.sh

# Default initialization
default_init

#number of events for each thread
export NEVENTS=$2

# Global tracking type to be used
export TRACKINGTYPE=$3

if [ "$1" = "muon" ] ; then
    create_output_dirs test_muons_reco_$TRACKINGTYPE/
    export DETECTORSETUP=muon
#                             NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU  E  PI PLUTO
    pars=(5    5    5   5   5    5    10     yes   yes no no no)
    set_simulation_parameters $pars
    set_default_muon_geometry
else

if [ "$1" = "electron" ] ; then
    create_output_dirs test_electrons_reco_$TRACKINGTYPE/
    export DETECTORSETUP=electron
#                             NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU E   PI PLUTO
    pars=(5    5    5   5   5    5    10     yes   no yes no no)
    set_simulation_parameters $pars    
    set_default_electron_geometry

fi
fi

for Y in 0; do
 for X in 0; do
  XXXX=00$Y$X
  export INFILE=$INFILEPREFIX/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
  set_default_file_names $DIR $XXXX
  
  root -b -q "./global_sim.C($NEVENTS)"
  root -b -q "./global_reco.C($NEVENTS, \"all\")"
  #root -b -q "./global_reco.C($NEVENTS, \"hits\")"
  #root -b -q "./global_reco.C($NEVENTS, \"tracking\")"
  #root -b -q "../rich/run/run_reco_rich.C($NEVENTS)"
  #root -b -q "../rich/electronId/run_electrons_qa.C($NEVENTS)"
  
#  . ./sim.sh
# bsub -q betch64 -J mc.$XXXX.run -o log/$XXXX.log -N -u andrey.lebedev@gsi.de sh much.sh
 done
done
export SCRIPT=no



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

