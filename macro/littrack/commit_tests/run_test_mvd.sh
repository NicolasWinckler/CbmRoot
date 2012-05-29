#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

mvd_delta=$1

create_output_dir events_mvd_${mvd_delta}/
create_result_dir results_mvd_${mvd_delta}/

nevents=5

set_default_mvd_geometry

export LIT_NOF_MVD_DELTA_EVENTS=0

export LIT_STS_HITPRODUCER_TYPE=real

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=${VMCWORKDIR}/input/urqmd.auau.25gev.centr.root

# Generate file for delta electrons
if [ "${mvd_delta}" = "delta" ] ; then
    #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
    pars=(0    0    0   0   0    0    0       0      1  no    no)
    set_simulation_parameters ${pars}
    export LIT_MC_FILE=${dir}/mc.delta.0000.root
    export LIT_PAR_FILE=${dir}/param.delta.0000.root
    ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(20)"
    export LIT_NOF_MVD_DELTA_EVENTS=10
    export LIT_MC_FILE=${dir}/mc.0000.root
    export LIT_PAR_FILE=${dir}/param.0000.root
fi

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
pars=(0    0    0   0   0    0    0       10      0  yes   yes)
set_simulation_parameters ${pars}   

export LIT_MC_FILE=${dir}/mc.0000.root
export LIT_PAR_FILE=${dir}/param.0000.root
 
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/mvd_reco.C(${nevents})"

export LIT_SCRIPT=no