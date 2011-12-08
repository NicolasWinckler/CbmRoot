#!/bin/sh

. $VMCWORKDIR/macro/littrack/common.sh

export SCRIPT=yes

create_output_dir events_muon/
create_result_dir results_muon/

export NEVENTS=2
export DETECTORSETUP=muon

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE URQMD UNIGEN
pars=(0    0    0   0   0    0    5       0      yes   yes)
set_simulation_parameters $pars

set_default_muon_geometry

export STSHITPRODUCERTYPE=real
export TRDHITPRODUCERTYPE=smearing
export MUCHHITPRODUCERTYPE=advanced
export GLOBALTRACKINGTYPE=branch

set_default_file_names $DIR 0000

export URQMDFILE=$VMCWORKDIR/input/urqmd.auau.25gev.centr.root
    
root -b -q -l "$VMCWORKDIR/macro/littrack/global_sim.C($NEVENTS)"
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"all\")"
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"hits\")"
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"

export SCRIPT=no
