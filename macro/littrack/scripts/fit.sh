#!/bin/sh

# Import common functionality for scripts
. ./common.sh

# Default initialization
default_init

#number of events for each thread
export NEVENTS=$2

if [ "$3" = "std" ] ; then
export TESTFASTPROPAGATION=0
else
if [ "$3" = "fast" ] ; then
export TESTFASTPROPAGATION=1
fi
fi

if [ "$1" = "muon" ] ; then
    create_output_dirs test_muons_fit_$3/
    export DETECTORSETUP=muon
#         NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU  E  PI PLUTO
    pars=(5    5    5   5   5    5    10     no    yes no no no)
    set_simulation_parameters $pars
    set_default_muon_geometry
else

if [ "$1" = "electron" ] ; then
    create_output_dirs test_electrons_fit_$3/ 
    export DETECTORSETUP=electron
#         NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU E   PI PLUTO
    pars=(5    5    5   5   5    5    10     no    no yes no no)
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
    root -b -q "./global_reco_ideal.C($NEVENTS)"
    root -b -q "./prop_ana.C($NEVENTS)"
  done
done
export SCRIPT=no
