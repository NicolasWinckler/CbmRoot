#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/u/andrey/cbm/trunk_w/build_new/

#Output directory for simulation files
export DIR=/d/cbm02/andrey/trdstd_10e_urqmd/

#Output directory for image files
export IMAGEDIR=./test/trdstd_10e_urqmd/

# CBM setup that will be used for simulation: "muon" or "electron"
export DETECTORSETUP=electron


export TRACKINGTYPE=branch

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
export MUONS=no
# If "yes" than primary electrons will be generated
export ELECTRONS=yes
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
export NEVENTS=500

for Y in 0; do
 for X in 0; do
  XXXX=00$Y$X
  echo batch $XXXX submitted
  export INFILE=/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
  export MCFILE=$DIR/mc.$XXXX.root
  export PARFILE=$DIR/param.$XXXX.root
  export GLOBALRECOFILE=$DIR/global.reco.$XXXX.root
  export GLOBALHITSFILE=$DIR/global.hits.$XXXX.root
  export GLOBALTRACKSFILE=$DIR/global.tracks.$XXXX.root
  
  COUNT=0
  while [ $COUNT -lt $NPLUTO ]; do
    echo PLUTO FILE COUNT IS: $COUNT
    export PLUTOFILE$COUNT=/home/d/andrey/pluto/Jpsi.000$COUNT.root
    let COUNT=COUNT+1    
  done 
  
  . ./sim.sh
# bsub -q betch64 -J mc.$XXXX.run -o log/$XXXX.log -N -u andrey.lebedev@gsi.de sh much.sh
 done
done
export SCRIPT=no
