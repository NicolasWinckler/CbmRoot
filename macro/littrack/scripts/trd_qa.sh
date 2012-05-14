#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1
trd_hitproducer_type=$2
trd_geom=$3
trd_digi=$4

create_output_dir events_${test_name}/
create_result_dir ${test_name}/

nevents=5
export LIT_DETECTOR_SETUP=electron

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE URQMD UNIGEN
pars=(0    0    0   0   0    0    0       10     yes   no)
set_simulation_parameters ${pars}    

set_default_electron_geometry

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_TRD_HITPRODUCER_TYPE=${trd_hitproducer_type}
export LIT_TRD_GEOM=${trd_geom}
export LIT_TRD_DIGI=${trd_digi}
export LIT_GLOBAL_TRACKING_TYPE=branch

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/Users/andrey/Development/cbm/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
    
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"all\")"
#${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"hits\")"
#${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"tracking\")"

export LIT_SCRIPT=no