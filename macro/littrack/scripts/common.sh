#!/bin/sh

# Function sets simulation parameters
function set_simulation_parameters() {
    pars=$1
    # number of embedded muons from FairBoxGenerator
    export NMUONSPLUS=${pars[0]}
    # number of embedded muons from FairBoxGenerator
    export NMUONSMINUS=${pars[1]}
    # number of embedded electrons from FairBoxGenerator
    export NELECTRONS=${pars[2]}
    # number of embedded positrons from FairBoxGenerator
    export NPOSITRONS=${pars[3]}
    # number of embedded pions from FairBoxGenerator
    export NPIONSPLUS=${pars[4]}
    # number of embedded pions from FairBoxGenerator
    export NPIONSMINUS=${pars[5]}
    # number of embedded J/Psi decayed to mu+ and mu- 
    export NJPSITOMUONS=${pars[6]}
    # number of embedded J/Psi decayed to e+ and e- 
    export NJPSITOELECTRONS=${pars[7]}
    # If "yes" than UrQMD will be used as background
    export URQMD=${pars[8]}
    # If "yes" than CbmUnigenGenerator will be useed instead of FairUrqmdGenerator
    export UNIGEN=${pars[9]}
}



# Function sets default file names using a specified DIR and file number
function set_default_file_names() {
    DIR=$1
    XXXX=$2
    export MCFILE=$DIR/mc.$XXXX.root
    export PARFILE=$DIR/param.$XXXX.root
    export GLOBALRECOFILE=$DIR/global.reco.$XXXX.root
    export GLOBALHITSFILE=$DIR/global.hits.$XXXX.root
    export GLOBALTRACKSFILE=$DIR/global.tracks.$XXXX.root
    export GLOBALTRACKSIDEALFILE=$DIR/global.tracks.ideal.$XXXX.root
    export MVDRECOFILE=$DIR/mvd.reco.$XXXX.root
    export PROPANAFILE=$DIR/propagation.ana.$XXXX.root
    export RICHFILE=$DIR/rich.reco.$XXXX.root
    export ELIDFILE=$DIR/elid.qa.$XXXX.root
}



# Function sets the default muon geometry
function set_default_muon_geometry() {
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_much.geo
    export SHIELDGEOM=shield_standard.geo
    export MVDGEOM=
    export STSGEOM=sts/sts_v11a.geo
    export STSDIGI=$VMCWORKDIR/parameters/sts/sts_v11a.digi.par
    export MUCHGEOM=much/much_v11a.geo
    export MUCHDIGI=$VMCWORKDIR/parameters/much/much_v11a.digi.root
    export RICHGEOM=
    export TRDGEOM=
    export TRDDIGI=
    export TOFGEOM=tof/tof_v07a.geo
    export ECALGEOM=
    export FIELDMAP=field_v10e
    export MAGNETGEOM=passive/magnet_v09m.geo
    
    export NORMSTSPOINTS=4
    export NORMTRDPOINTS=0
    export NORMMUCHPOINTS=16
    export NORMTOFPOINTS=1
    export NORMTRDHITS=0
    export NORMMUCHHITS=15
    export NORMTOFHITS=1
}



# Function sets the default electron geometry
function set_default_electron_geometry() {
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_standard.geo
    export SHIELDGEOM=
    export MVDGEOM=
    export STSGEOM=sts/sts_v11a.geo
    export STSDIGI=$VMCWORKDIR/parameters/sts/sts_v11a.digi.par
    export MUCHGEOM=
    export MUCHDIGI=
    export RICHGEOM=rich/rich_v08a.geo
    export TRDGEOM=trd/trd_v10b.geo
    export TRDDIGI=$VMCWORKDIR/parameters/trd/trd_v10b.digi.par
    export TOFGEOM=tof/tof_v07a.geo
    export ECALGEOM=
    export FIELDMAP=field_v10e
    export MAGNETGEOM=passive/magnet_v09e.geo
    
    export NORMSTSPOINTS=4
    export NORMTRDPOINTS=8
    export NORMMUCHPOINTS=0
    export NORMTOFPOINTS=1
    export NORMTRDHITS=8
    export NORMMUCHHITS=0
    export NORMTOFHITS=1
}



# Function set default mvd geometry
function set_default_mvd_geometry() {
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_much.geo
    export SHIELDGEOM=shield_standard.geo
    export MVDGEOM=mvd/mvd_v07a.geo
    export STSGEOM=sts/sts_v11a.geo
    export STSDIGI=$VMCWORKDIR/parameters/sts/sts_v11a.digi.par
    export MUCHGEOM=
    export MUCHDIGI=
    export RICHGEOM=
    export TRDGEOM=
    export TRDDIGI=
    export TOFGEOM=
    export ECALGEOM=
    export FIELDMAP=field_v10e
    export MAGNETGEOM=passive/magnet_v09m.geo
    
    export NORMSTSPOINTS=4
    export NORMTRDPOINTS=0
    export NORMMUCHPOINTS=0
    export NORMTOFPOINTS=0
    export NORMTRDHITS=0
    export NORMMUCHHITS=0
    export NORMTOFHITS=0
}



# Function exports and creates output directories for DIR
function create_output_dir()
{
    export DIR=$1
    rm -r -f $DIR
    mkdir $DIR
}



# Function exports and creates output directories for RESULTDIR
function create_result_dir()
{
    export RESULTDIR=$1
    rm -r -f $RESULTDIR
    mkdir $RESULTDIR
}

