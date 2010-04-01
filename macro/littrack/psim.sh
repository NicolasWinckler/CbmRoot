#!/bin/sh
#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/u/andrey/cbm/trunk_phd/build_new/

cd $MYBUILDDIR
. ./config.sh
cd -

# CBM setup that will be used for simulation: "muon" or "electron"
export DETECTORSETUP=muon

#Simulation parameters
# number of embedded muons
export NMUONSPLUS=5
# number of embedded muons
export NMUONSMINUS=5
# number of embedded electrons
export NELECTRONS=5
# number of embedded positrons
export NPOSITRONS=5
# number of embedded pions
export NPIONSPLUS=5
# number of embedded pions
export NPIONSMINUS=5
# number of embedded particles from pluto. MAX = 10.
export NPLUTO=10
# If "yes" than UrQMD will be used as background
export URQMD=yes
# If "yes" than primary muons will be generated
export MUONS=yes
# If "yes" than primary electrons will be generated
export ELECTRONS=no
# If "yes" than primary pions will be generated
export PIONS=no
# If "yes" PLUTO particles will be embedded
export PLUTO=no

# Geometries
if [ $DETECTORSETUP = "muon" ]; then
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_much.geo
    export SHIELDGEOM=shield_standard.geo
    export MVDGEOM=
    export STSGEOM=sts_standard.geo
    export MUCHGEOM=much_standard_2layers.geo
    #MUCH digitization file
    export MUCHDIGI=$VMCWORKDIR/parameters/much/much_standard_2layers.digi.root
    export RICHGEOM=
    export TRDGEOM=
#trd_muon_setup_new.geo   
    export TOFGEOM=tof_standard.geo
    export ECALGEOM=
    export FIELDMAP=field_muon_standard
    export MAGNETGEOM=magnet_muon_standard.geo
else
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_much.geo
    export SHIELDGEOM=shield_standard.geo
    export MVDGEOM= #mvd_standard.geo
    export STSGEOM=sts_standard.geo
    export MUCHGEOM=
    export RICHGEOM=rich_standard.geo
    export TRDGEOM=trd_standard.geo
    export TOFGEOM=tof_standard.geo
    export ECALGEOM=
    export FIELDMAP=field_electron_standard
    export MAGNETGEOM=magnet_electron_standard.geo
fi

#number of events for each thread
export NEVENTS=1
#number of events for track fit test
export NEVENTS2=10

export XXXX=0000
export INFILE=/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14

export DIRPREFIX=/d/cbm02/andrey/phd

#Specific for different detectors
function run {
    //change geometry
    export GEONAME=$1
    export MUCHGEOM=$2
    export MUCHDIGI=$3
    export TRDGEOM=$4
    export SHIELDGEOM=$5
    
    //settings for simulation with urqmd
    TMP=_10mu_urqmd/
    export DIR=$DIRPREFIX/$GEONAME$TMP
    rm -r $DIR
    mkdir $DIR
    export MCFILE=$DIR/mc.$XXXX.root
    export PARFILE=$DIR/param.$XXXX.root
    export GLOBALRECOFILE=$DIR/global.reco.$XXXX.root
    export GLOBALHITSFILE=$DIR/global.hits.$XXXX.root
    
    root -b -q "./global_sim.C($NEVENTS)"
    root -b -q "./global_hits.C($NEVENTS)"
    
    export TRACKINGTYPE=branch 
    export GLOBALTRACKSFILE=$DIR/global.tracks.$TRACKINGTYPE.$XXXX.root
    export IMAGEDIR=./test/$GEONAME/$TRACKINGTYPE/
    rm -r $IMAGEDIR
    mkdir $IMAGEDIR
    root -b -q "./global_tracking.C($NEVENTS)"
    
    export TRACKINGTYPE=nn
    export GLOBALTRACKSFILE=$DIR/global.tracks.$TRACKINGTYPE.$XXXX.root
    export IMAGEDIR=./test/$GEONAME/$TRACKINGTYPE/
    rm -r $IMAGEDIR
    mkdir $IMAGEDIR
    root -b -q "./global_tracking.C($NEVENTS)"
    
    export TRACKINGTYPE=weight
    export GLOBALTRACKSFILE=$DIR/global.tracks.$TRACKINGTYPE.$XXXX.root
    export IMAGEDIR=./test/$GEONAME/$TRACKINGTYPE/
    mkdir $IMAGEDIR
    root -b -q "./global_tracking.C($NEVENTS)"
    
    TMP2=_10mu/
    export DIR=$DIRPREFIX/$GEONAME$TMP2
    rm -r $DIR
    mkdir $DIR
    export MCFILE=$DIR/mc.$XXXX.root
    export PARFILE=$DIR/param.$XXXX.root
    export GLOBALTRACKSIDEALFILE=$DIR/global.reco.ideal.$XXXX.root
    export PROPANAFILE=$DIR/propagation.ana.$XXXX.root
    export IMAGEDIR=./test/$GEONAME/
    export URQMD=no
    export MUONS=yes
    root -b -q "./global_sim.C($NEVENTS2)"
    root -b -q "./global_reco_ideal.C($NEVENTS2)"
#    root -b -q "./prop_ana.C($NEVENTS2)"
}

if [ $1 -eq 1 ] ; then
    run much1 much_standard.geo $VMCWORKDIR/parameters/much/much_standard.digi.root "" shield_standard.geo
else                   

if [ $1 -eq 2 ] ; then
    run much2 much_standard_2layers.geo $VMCWORKDIR/parameters/much/much_standard_2layers.digi.root "" shield_standard.geo
else
         
if [ $1 -eq 3 ] ; then
   run much3 much_standard_straw_2layers.geo $VMCWORKDIR/parameters/much/much_standard_straw_2layers.digi.root "" shield_standard.geo
else

if [ $1 -eq 4 ] ; then
   run much4 much_standard_trd_2layers.geo $VMCWORKDIR/parameters/much/much_standard_trd_2layers.digi.root trd_muon_setup_new.geo shield_standard_trd.geo
else

if [ $1 -eq 5 ] ; then
   run much5 much_standard_straw_trd_2layers.geo $VMCWORKDIR/parameters/much/digi_straw_trd_2layers.root trd_muon_setup_new.geo shield_standard_trd.geo
fi
fi
fi   
fi
fi




export SCRIPT=no
