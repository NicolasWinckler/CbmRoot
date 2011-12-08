#!/bin/sh

. $VMCWORKDIR/macro/littrack/common.sh

export SCRIPT=yes

create_output_dir events_muon/

export NEVENTS=2
export DETECTORSETUP=muon

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE URQMD UNIGEN
pars=(0    0    0   0   0    0    5       0      yes   no)
set_simulation_parameters $pars  

set_default_muon_geometry

export STSHITPRODUCERTYPE=real
export TRDHITPRODUCERTYPE=smearing
export MUCHHITPRODUCERTYPE=advanced

set_default_file_names $DIR 0000

export URQMDFILE=/data.local1/andrey/tests/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14

root -b -q -l "$VMCWORKDIR/macro/littrack/global_sim.C($NEVENTS)"
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"hits\")"

# Test different global tracking algorithms
# Branching algorithm
export GLOBALTRACKINGTYPE=branch
create_result_dir results_muon_branch/
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"
# Nearest neighbour algorithm
export GLOBALTRACKINGTYPE=nn
create_result_dir results_muon_nn/
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"
# Nearest neighbour parallel algorithm
export GLOBALTRACKINGTYPE=nn_parallel
create_result_dir results_muon_nn_parallel/
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"

export SCRIPT=no
