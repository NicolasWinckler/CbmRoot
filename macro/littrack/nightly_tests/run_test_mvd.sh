#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1
system=$2
energy=$3
collision_type=$4
mvd_delta=$5
nevents=500

create_output_dir events_${test_name}/
create_result_dir ${test_name}/

export LIT_NOF_MVD_DELTA_EVENTS=0

set_default_mvd_geometry

export LIT_STS_HITPRODUCER_TYPE=real

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/data.local1/andrey/tests/urqmd/${system}/${energy}/${collision_type}/urqmd.${system}.${energy}.${collision_type}.0001.ftn14

# Generate file for delta electrons
if [ "${mvd_delta}" = "delta" ] ; then
    #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
    pars=(0    0    0   0   0    0    0       0      1  no    no)
    set_simulation_parameters ${pars}
    export LIT_MC_FILE=${dir}/mc.delta.0000.root
    export LIT_PAR_FILE=${dir}/param.delta.0000.root
    ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(600)"
    export LIT_NOF_MVD_DELTA_EVENTS=300
    export LIT_MC_FILE=${dir}/mc.0000.root
    export LIT_PAR_FILE=${dir}/param.0000.root
fi

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
pars=(0    0    0   0   0    0    0       10     0  yes   no)
set_simulation_parameters ${pars} 

${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/mvd_reco.C(${nevents})"

export LIT_SCRIPT=no
