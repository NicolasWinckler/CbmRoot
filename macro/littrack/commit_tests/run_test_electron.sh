#!/bin/sh

. $VMCWORKDIR/macro/littrack/scripts/common.sh

export SCRIPT=yes

create_output_dir events_electron/
create_result_dir results_electron/

export NEVENTS=5
export DETECTORSETUP=electron

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE URQMD UNIGEN
pars=(0    0    0   0   0    0    0       5      yes   yes)
set_simulation_parameters $pars    

set_default_electron_geometry

export STSHITPRODUCERTYPE=real
export TRDHITPRODUCERTYPE=smearing
export MUCHHITPRODUCERTYPE=advanced
export GLOBALTRACKINGTYPE=branch

set_default_file_names $DIR 0000

export URQMDFILE=$VMCWORKDIR/input/urqmd.auau.25gev.centr.root
    
$ROOTSYS/bin/root -b -q -l "$VMCWORKDIR/macro/littrack/global_sim.C($NEVENTS)"
$ROOTSYS/bin/root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"all\")"
$ROOTSYS/bin/root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"hits\")"
$ROOTSYS/bin/root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"

export SCRIPT=no
