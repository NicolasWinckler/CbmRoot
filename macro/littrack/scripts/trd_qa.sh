#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1
trd_geom=$2
trd_digi=$3

create_output_dir events_${test_name}/

nevents=500
export LIT_DETECTOR_SETUP=electron

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
pars=(0    0    0   0   0    0    0       10     0  yes   no)
set_simulation_parameters ${pars}    

set_default_electron_geometry

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_TRD_GEOM=${trd_geom}
export LIT_TRD_DIGI=${trd_digi}
export LIT_GLOBAL_TRACKING_TYPE=branch

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/Users/andrey/Development/cbm/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
    
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"

function run_reco() {
   export LIT_TRD_HITPRODUCER_TYPE=$1
   export LIT_GLOBAL_TRACKING_TYPE=$2
   export LIT_GLOBAL_RECO_FILE=${LIT_DIR}/global.reco.${LIT_TRD_HITPRODUCER_TYPE}.${LIT_GLOBAL_TRACKING_TYPE}.0000.root
   create_result_dir ${test_name}_${LIT_TRD_HITPRODUCER_TYPE}_${LIT_GLOBAL_TRACKING_TYPE}/
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"all\")"
}

run_reco smearing nn
run_reco smearing branch
run_reco clustering nn
run_reco clustering branch

export LIT_SCRIPT=no