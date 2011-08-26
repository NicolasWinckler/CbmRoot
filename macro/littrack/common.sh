#!/bin/sh

# Default initialisation
function default_init() {
    #Macro will be running via script
    export SCRIPT=yes
    
    #Prefix for output directory
    export DIRPREFIX=/d/cbm02/andrey/test
    
    #Prefix for output directory
    export IMAGEDIRPREFIX=/d/cbm02/andrey/test
    
    #Prefix for input UrQMD file
    export INFILEPREFIX=/d/cbm03/urqmd
    
    #Build directory of CBMROOT
    export MYBUILDDIR=/u/andrey/cbm/trunk/build_lxir039/
    
    cd $MYBUILDDIR
    . ./config.sh
    cd -
}



# Function sets simulation parameters
function set_simulation_parameters() {
    pars=$1
    # number of embedded muons
    export NMUONSPLUS=${pars[0]}
    # number of embedded muons
    export NMUONSMINUS=${pars[1]}
    # number of embedded electrons
    export NELECTRONS=${pars[2]}
    # number of embedded positrons
    export NPOSITRONS=${pars[3]}
    # number of embedded pions
    export NPIONSPLUS=${pars[4]}
    # number of embedded pions
    export NPIONSMINUS=${pars[5]}
    # number of embedded particles from pluto. MAX = 10.
    export NPLUTO=${pars[6]}
    # If "yes" than UrQMD will be used as background
    export URQMD=${pars[7]}
    # If "yes" than primary muons will be generated
    export MUONS=${pars[8]}
    # If "yes" than primary electrons will be generated
    export ELECTRONS=${pars[9]}
    # If "yes" than primary pions will be generated
    export PIONS=${pars[10]}
    # If "yes" PLUTO particles will be embedded
    export PLUTO=${pars[11]}
    # If "yes" than CbmUnigenGenerator willbe useed instead of FairUrqmdGenerator
    export USEUNIGEN=${pars[12]}
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
    export PROPANAFILE=$DIR/propagation.ana.$XXXX.root
    export RICHFILE=$DIR/rich.reco.$XXXX.root
    export ELIDFILE=$DIR/elid.qa.$XXXX.root
  
    COUNT=0
    while [ $COUNT -lt $NPLUTO ]; do
        echo PLUTO FILE COUNT IS: $COUNT
        export PLUTOFILE$COUNT=/home/d/andrey/pluto/Jpsi.000$COUNT.root
        let COUNT=COUNT+1    
    done 
}



# Function sets the default muon geometry
function set_default_muon_geometry() {
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_much.geo
    export SHIELDGEOM=shield_standard.geo
    export MVDGEOM=
    export STSGEOM=sts/sts_v09a.geo
    export STSDIGI=$VMCWORKDIR/parameters/sts/sts_standard.digi.par
    export MUCHGEOM=much/much_v11a.geo
    export MUCHDIGI=$VMCWORKDIR/parameters/much/much_v11a.digi.root
    export RICHGEOM=
    export TRDGEOM=
#trd_muon_setup_new.geo
    export TRDDIGI=
    export TOFGEOM=tof/tof_v07a.geo
    export ECALGEOM=
    export FIELDMAP=field_v10e
    export MAGNETGEOM=passive/magnet_v09m.geo
    
    export NOFTRDHITS=0
    export NOFMUCHHITS=13
    export NOFTOFHITS=1
    export PDG=13
    
    export NORMSTSPOINTS=4
    export NORMTRDPOINTS=0
    export NORMMUCHPOINTS=16
    export NORMTOFPOINTS=1
    export NORMTRDHITS=0
    export NORMMUCHHITS=15
    export NORMTOFHITS=1
    
    export MOMMIN=0
    export MOMMAX=16
    export MOMBINS=16
}



# Function sets the default electron geometry
function set_default_electron_geometry() {
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_standard.geo
    export SHIELDGEOM=
    export MVDGEOM=
    export STSGEOM=sts/sts_v09a.geo
    export STSDIGI=$VMCWORKDIR/parameters/sts/sts_standard.digi.par
    export MUCHGEOM=
    export MUCHDIGI=
    export RICHGEOM=rich/rich_v08a.geo
    export TRDGEOM=trd/trd_v10b.geo
    export TRDDIGI=$VMCWORKDIR/parameters/trd/trd_v10b.digi.par
    export TOFGEOM=tof/tof_v07a.geo
    export ECALGEOM=
    export FIELDMAP=field_v10e
    export MAGNETGEOM=passive/magnet_v09e.geo
    
    export NOFTRDHITS=12
    export NOFMUCHHITS=0
    export NOFTOFHITS=1
    export PDG=11
    
    export NORMSTSPOINTS=4
    export NORMTRDPOINTS=8
    export NORMMUCHPOINTS=0
    export NORMTOFPOINTS=1
    export NORMTRDHITS=8
    export NORMMUCHHITS=0
    export NORMTOFHITS=1
    
    export MOMMIN=0
    export MOMMAX=12
    export MOMBINS=12
}



# Function exports and creates output directories for DIR
function create_output_dir()
{
    export DIR=$1
    rm -r $DIR
    mkdir $DIR
}



# Function exports and creates output directories for IMAGEDIR
function create_image_dir()
{
    export IMAGEDIR=$1
    rm -r $IMAGEDIR
    mkdir $IMAGEDIR
}

