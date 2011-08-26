#!/bin/sh

# Track reconstruction tests
#xterm -hold -e ". ./sim.sh muon 100 branch"&
#xterm -hold -e ". ./sim.sh electron 100 branch"&
#xterm -hold -e ". ./sim.sh muon 100 nn"&
#xterm -hold -e ". ./sim.sh electron 100 nn"&
#xterm -hold -e ". ./sim.sh muon 100 nn_parallel"&
#xterm -hold -e ". ./sim.sh electron 100 nn_scalar"&

# Track fit tests
#xterm -hold -e ". ./fit.sh muon 100 std"&
#xterm -hold -e ". ./fit.sh electron 100 std"&
#xterm -hold -e ". ./fit.sh muon 100 fast"&
#xterm -hold -e ". ./fit.sh electron 100 fast"&

#export NEVENTS=500
#root -b -q "./global_sim.C($NEVENTS)"
#root -b -q "./global_reco.C($NEVENTS, \"hits\")"
#root -b -q "./global_reco.C($NEVENTS, \"tracking\")"




# TRD test of differen hit producers
. ./common.sh
default_init
export NEVENTS=1000

create_output_dir /d/cbm02/andrey/electron/v10b_10e_urqmd

export DETECTORSETUP=electron

#     NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU E   PI  PLUTO USEUNIGEN
pars=(5    5    5   5   20   20   10     no    no yes yes  no    no)
set_simulation_parameters $pars    

set_default_electron_geometry

export TRACKINGTYPE=branch

export INFILE=$INFILEPREFIX/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
export MCFILE=$DIR/mc.0000.root
export PARFILE=$DIR/param.0000.root
    
root -b -q "./global_sim.C($NEVENTS)"

. ./trd_sim.sh $NEVENTS trd_10e_urqmd_smearing/ $VMCWORKDIR/parameters/trd/trd_v10b.digi.par smearing smearing
. ./trd_sim.sh $NEVENTS trd_10e_urqmd_digi/ $VMCWORKDIR/parameters/trd/trd_v10b.digi.par digi digi
. ./trd_sim.sh $NEVENTS trd_10e_urqmd_clustering/ $VMCWORKDIR/parameters/trd/trd_v10b.digi.par clustering clustering
. ./trd_sim.sh $NEVENTS trd_10e_urqmd_digi_1cm2/ $VMCWORKDIR/parameters/trd/trd.v10b.1cm2.digi.par digi digi.1cm2
. ./trd_sim.sh $NEVENTS trd_10e_urqmd_clustering_1cm2/ $VMCWORKDIR/parameters/trd/trd.v10b.1cm2.digi.par clustering clustering.1cm2


