#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=littrack_mvd_reco
nevents=500

create_output_dir events_${test_name}/
create_result_dir ${test_name}/

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE URQMD UNIGEN
pars=(0    0    0   0   0    0    0       5      yes   no)
set_simulation_parameters ${pars} 
set_default_mvd_geometry

export LIT_STS_HITPRODUCER_TYPE=real

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/data.local1/andrey/tests/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0001.ftn14

${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/mvd_reco.C(${nevents})"

export LIT_SCRIPT=no
