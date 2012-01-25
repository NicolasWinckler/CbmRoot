#!/bin/sh

export SCRIPT=yes

. $VMCWORKDIR/macro/littrack/common.sh

export NEVENTS=$1
export DETECTORSETUP=electron

create_output_dir /data.local1/andrey/events/trd_test_jan12/events_$2/
create_result_dir /data.local1/andrey/events/trd_test_jan12/results_$2/

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE URQMD UNIGEN
pars=(0    0    0   0   0    0    0       5      yes   no)
set_simulation_parameters $pars 

set_default_electron_geometry

export GLOBALTRACKINGTYPE=branch
export STSHITPRODUCERTYPE=real
export TRDHITPRODUCERTYPE=$3
export TRDGEOM=$4
export TRDDIGI=$5

set_default_file_names $DIR 0000
export URQMDFILE=/data.local1/andrey/tests/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14

$ROOTSYS/bin/root -b -q "./global_sim.C($NEVENTS)"
$ROOTSYS/bin/root -b -q "./global_reco.C($NEVENTS, \"all\")"
#$ROOTSYS/bin/root -b -q "./global_reco.C($NEVENTS, \"hits\")"
#$ROOTSYS/bin/root -b -q "./global_reco.C($NEVENTS, \"tracking\")"

export SCRIPT=no
