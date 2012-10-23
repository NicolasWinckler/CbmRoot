#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

XXXX=$1

#main_dir=/lustre/cbm/user/andrey/events/nu/25gev/
nevents=10

#create_output_dir events_${test_name}/
create_result_dir ${MAIN_DIR}/results/${XXXX}/

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
pars=(0    0    0   0   0    0    0       0      0  yes   no)
set_simulation_parameters ${pars}    

set_default_sts_tof_geometry

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_GLOBAL_TRACKING_TYPE=branch

set_default_file_names ${MAIN_DIR} ${XXXX}
export LIT_URQMD_FILE=/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.${XXXX}.ftn14

${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"all\")"

export LIT_SCRIPT=no
