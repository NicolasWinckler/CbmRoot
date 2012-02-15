#!/bin/sh

. $VMCWORKDIR/macro/littrack/common.sh

export SCRIPT=yes

TEST_NAME=std_electron_reco

create_output_dir events_$TEST_NAME/

export NEVENTS=10
export DETECTORSETUP=electron 

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE URQMD UNIGEN
pars=(0    0    0   0   0    0    0       5      yes   no)
set_simulation_parameters $pars   

set_default_electron_geometry

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
create_result_dir $TEST_NAME_$GLOBALTRACKINGTYPE/
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"
# Nearest neighbour algorithm
export GLOBALTRACKINGTYPE=nn
create_result_dir $TEST_NAME_$GLOBALTRACKINGTYPE/
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"
# Nearest neighbour parallel algorithm
export GLOBALTRACKINGTYPE=nn_parallel
create_result_dir $TEST_NAME_$GLOBALTRACKINGTYPE/
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"

# Run summary report generator
export STUDYOUTPUTDIR=summary_report$TEST_NAME
rm -rf $STUDYOUTPUTDIR
mkdir $STUDYOUTPUTDIR
export NSTUDIES=3
export STUDYNAME1=branch
export STUDYRESULT1=$TEST_NAME_$STUDYNAME1/
export STUDYNAME2=nn
export STUDYRESULT2=$TEST_NAME_$STUDYNAME2/
export STUDYNAME3=nn_parallel
export STUDYRESULT3=$TEST_NAME_$STUDYNAME3/
root -b -q -l "$VMCWORKDIR/macro/littrack/qa_study.C"

export SCRIPT=no
