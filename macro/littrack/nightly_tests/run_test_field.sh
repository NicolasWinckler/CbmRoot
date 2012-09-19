#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

create_output_dir events_field/
create_result_dir littrack_field/

nevents=1

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
pars=(0    0    0   0   0    0    0       5      0  no    no)
set_simulation_parameters ${pars}    

set_default_electron_geometry

export LIT_FIELD_MAP=field_v10e
export LIT_MAGNET_GEOM=passive/magnet_v09e.geo

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=${VMCWORKDIR}/input/urqmd.auau.25gev.centr.root
    
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/field_qa.C"

export LIT_SCRIPT=no
