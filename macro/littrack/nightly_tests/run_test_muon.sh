#!/bin/sh

echo $PATH
echo $LD_LIBRARY_PATH
which root

. $VMCWORKDIR/macro/littrack/common.sh

export SCRIPT=yes

create_output_dir events_muon/

export NEVENTS=500
export DETECTORSETUP=muon

#     NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU  E   PI  PLUTO USEUNIGEN
pars=(5    5    5   5   20   20   10     yes   yes no  no  no    no)
set_simulation_parameters $pars    

set_default_muon_geometry

export STSHITPRODUCERTYPE=real
export TRDHITPRODUCERTYPE=smearing

set_default_file_names $DIR 0000

#export INFILE=$VMCWORKDIR/input/urqmd.auau.25gev.centr.root
export INFILE=/data.local1/andrey/tests/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14

root -b -q -l "$VMCWORKDIR/macro/littrack/global_sim.C($NEVENTS)"
#root -b -q "../global_reco.C($NEVENTS, \"all\")"
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"hits\")"

# Test different global tracking algorithms
# Branching algorithm
export TRACKINGTYPE=branch
create_image_dir results_muon_branch/
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"
# Nearest neighbour algorithm
export TRACKINGTYPE=nn
create_image_dir results_muon_nn/
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"
# Nearest neighbour parallel algorithm
export TRACKINGTYPE=nn_parallel
create_image_dir results_muon_nn_parallel/
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"

export SCRIPT=no
