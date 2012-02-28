#!/bin/sh

# Function sets simulation parameters
function set_simulation_parameters() {
    pars=$1
    # number of embedded muons from FairBoxGenerator
    export LIT_NOF_MUONS_PLUS=${pars[0]}
    # number of embedded muons from FairBoxGenerator
    export LIT_NOF_MUONS_MINUS=${pars[1]}
    # number of embedded electrons from FairBoxGenerator
    export LIT_NOF_ELECTRONS=${pars[2]}
    # number of embedded positrons from FairBoxGenerator
    export LIT_NOF_POSITRONS=${pars[3]}
    # number of embedded pions from FairBoxGenerator
    export LIT_NOF_PIONS_PLUS=${pars[4]}
    # number of embedded pions from FairBoxGenerator
    export LIT_NOF_PIONS_MINUS=${pars[5]}
    # number of embedded J/Psi decayed to mu+ and mu- 
    export LIT_NOF_JPSI_TO_MUONS=${pars[6]}
    # number of embedded J/Psi decayed to e+ and e- 
    export LIT_NOF_JPSI_TO_ELECTRONS=${pars[7]}
    # If "yes" than UrQMD will be used as background
    export LIT_URQMD=${pars[8]}
    # If "yes" than CbmUnigenGenerator will be useed instead of FairUrqmdGenerator
    export LIT_UNIGEN=${pars[9]}
}



# Function sets default file names using a specified DIR and file number
function set_default_file_names() {
    dir=$1
    XXXX=$2
    export LIT_MC_FILE=${dir}/mc.${XXXX}.root
    export LIT_PAR_FILE=${dir}/param.${XXXX}.root
    export LIT_GLOBAL_RECO_FILE=${dir}/global.reco.${XXXX}.root
    export LIT_GLOBAL_HITS_FILE=${dir}/global.hits.${XXXX}.root
    export LIT_GLOBAL_TRACKS_FILE=${dir}/global.tracks.${XXXX}.root
    export LIT_GLOBAL_TRACKS_IDEAL_FILE=${dir}/global.tracks.ideal.${XXXX}.root
    export LIT_MVD_RECO_FILE=${dir}/mvd.reco.${XXXX}.root
    export LIT_PROP_ANA_FILE=${dir}/propagation.ana.${XXXX}.root
    export LIT_RICH_FILE=${dir}/rich.reco.${XXXX}.root
    export LIT_ELID_FILE=${dir}/elid.qa.${XXXX}.root
}



# Function sets the default muon geometry
function set_default_muon_geometry() {
    export LIT_CAVE_GEOM=cave.geo
    export LIT_TARGET_GEOM=target_au_250mu.geo
    export LIT_PIPE_GEOM=pipe_much.geo
    export LIT_SHIELD_GEOM=shield_standard.geo
    export LIT_MVD_GEOM=
    export LIT_STS_GEOM=sts/sts_v11a.geo
    export LIT_STS_DIGI=${VMCWORKDIR}/parameters/sts/sts_v11a.digi.par
    export LIT_MUCH_GEOM=much/much_v11a.geo
    export LIT_MUCH_DIGI=${VMCWORKDIR}/parameters/much/much_v11a.digi.root
    export LIT_RICH_GEOM=
    export LIT_TRD_GEOM=
    export LIT_TRD_DIGI=
    export LIT_TOF_GEOM=tof/tof_v07a.geo
    export LIT_ECAL_GEOM=
    export LIT_FIELD_MAP=field_v10e
    export LIT_MAGNET_GEOM=passive/magnet_v09m.geo
    
    export LIT_NORM_STS_POINTS=4
    export LIT_NORM_TRD_POINTS=0
    export LIT_NORM_MUCH_POINTS=16
    export LIT_NORM_TOF_POINTS=1
    export LIT_NORM_TRD_HITS=0
    export LIT_NORM_MUCH_HITS=15
    export LIT_NORM_TOF_HITS=1
}



# Function sets the default electron geometry
function set_default_electron_geometry() {
    export LIT_CAVE_GEOM=cave.geo
    export LIT_TARGET_GEOM=target_au_250mu.geo
    export LIT_PIPE_GEOM=pipe_standard.geo
    export LIT_SHIELD_GEOM=
    export LIT_MVD_GEOM=
    export LIT_STS_GEOM=sts/sts_v11a.geo
    export LIT_STS_DIGI=${VMCWORKDIR}/parameters/sts/sts_v11a.digi.par
    export LIT_MUCH_GEOM=
    export LIT_MUCH_DIGI=
    export LIT_RICH_GEOM=rich/rich_v08a.geo
    export LIT_TRD_GEOM=trd/trd_v10b.geo
    export LIT_TRD_DIGI=${VMCWORKDIR}/parameters/trd/trd_v10b.digi.par
    export LIT_TOF_GEOM=tof/tof_v07a.geo
    export LIT_ECAL_GEOM=
    export LIT_FIELD_MAP=field_v10e
    export LIT_MAGNET_GEOM=passive/magnet_v09e.geo
    
    export LIT_NORM_STS_POINTS=4
    export LIT_NORM_TRD_POINTS=8
    export LIT_NORM_MUCH_POINTS=0
    export LIT_NORM_TOF_POINTS=1
    export LIT_NORM_TRD_HITS=8
    export LIT_NORM_MUCH_HITS=0
    export LIT_NORM_TOF_HITS=1
}



# Function set default mvd geometry
function set_default_mvd_geometry() {
    export LIT_CAVE_GEOM=cave.geo
    export LIT_TARGET_GEOM=target_au_250mu.geo
    export LIT_PIPE_GEOM=pipe_much.geo
    export LIT_SHIELD_GEOM=shield_standard.geo
    export LIT_MVD_GEOM=mvd/mvd_v07a.geo
    export LIT_STS_GEOM=sts/sts_v11a.geo
    export LIT_STS_DIGI=${VMCWORKDIR}/parameters/sts/sts_v11a.digi.par
    export LIT_MUCH_GEOM=
    export LIT_MUCH_DIGI=
    export LIT_RICH_GEOM=
    export LIT_TRD_GEOM=
    export LIT_TRD_DIGI=
    export LIT_TOF_GEOM=
    export LIT_ECAL_GEOM=
    export LIT_FIELD_MAP=field_v10e
    export LIT_MAGNET_GEOM=passive/magnet_v09m.geo
    
    export LIT_NORM_STS_POINTS=4
    export LIT_NORM_TRD_POINTS=0
    export LIT_NORM_MUCH_POINTS=0
    export LIT_NORM_TOF_POINTS=0
    export LIT_NORM_TRD_HITS=0
    export LIT_NORM_MUCH_HITS=0
    export LIT_NORM_TOF_HITS=0
}



# Function exports and creates output directories for LIT_DIR
function create_output_dir()
{
    export LIT_DIR=$1
    rm -r -f ${LIT_DIR}
    mkdir ${LIT_DIR}
}



# Function exports and creates output directories for LIT_RESULT_DIR
function create_result_dir()
{
    export LIT_RESULT_DIR=$1
    rm -r -f ${LIT_RESULT_DIR}
    mkdir ${LIT_RESULT_DIR}
}
