#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1

create_output_dir events_${test_name}/

nevents=500
export LIT_DETECTOR_SETUP=$2

if [ "${LIT_DETECTOR_SETUP}" = "electron" ] ; then
    #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE URQMD UNIGEN
    pars=(0    0    0   0   0    0    0       5      yes   no)
    set_default_electron_geometry
elif [ "${LIT_DETECTOR_SETUP}" = "muon" ] ; then
    #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE URQMD UNIGEN
    pars=(0    0    0   0   0    0    5       0      yes   no)
    set_default_electron_geometry
else
    echo "Error! Detector setup unknown! Must be electron or muon!"
    exit 2
fi

set_simulation_parameters ${pars} 

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_TRD_HITPRODUCER_TYPE=smearing
export LIT_MUCH_HITPRODUCER_TYPE=advanced

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/data.local1/andrey/tests/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0001.ftn14

$ROOTSYS/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
$ROOTSYS/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"hits\")"

# Test different global tracking algorithms
# Branching algorithm
export LIT_GLOBAL_TRACKING_TYPE=branch
create_result_dir ${test_name}_${LIT_GLOBAL_TRACKING_TYPE}/
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"tracking\")"
# Nearest neighbour algorithm
export LIT_GLOBAL_TRACKING_TYPE=nn
create_result_dir ${test_name}_${LIT_GLOBAL_TRACKING_TYPE}/
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"tracking\")"
# Nearest neighbour parallel algorithm
export LIT_GLOBAL_TRACKING_TYPE=nn_parallel
create_result_dir ${test_name}_${LIT_GLOBAL_TRACKING_TYPE}/
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"tracking\")"

# Run summary report generator
export LIT_STUDY_OUTPUT_DIR=${test_name}_summary
rm -rf ${LIT_STUDY_OUTPUT_DIR}
mkdir ${LIT_STUDY_OUTPUT_DIR}
export LIT_NOF_STUDIES=3
export LIT_STUDY_NAME1=branch
export LIT_STUDY_RESULT1=${test_name}_${LIT_STUDY_NAME1}/
export LIT_STUDY_NAME2=nn
export LIT_STUDY_RESULT2=${test_name}_${LIT_STUDY_NAME2}/
export LIT_STUDY_NAME3=nn_parallel
export LIT_STUDY_RESULT3=${test_name}_${LIT_STUDY_NAME3}/
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/qa_study.C"

export LIT_SCRIPT=no
