#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/u/slebedev/JUL09/buildSC

#Output directory for simulation files
export DIR=/d/cbm02/slebedev/rich/JUL09/test_electrons

#Output directory for image files
export IMAGEDIR=/u/slebedev/JUL09/trunk/macro/rich/test_electrons_images/

export TRACKINGTYPE=branch

#Simulation parameters
# number of embedded electrons
export NELECTRONS=5
# number of embedded positrons
export NPOSITRONS=5
# If "yes" than UrQMD will be used as background
export URQMD=yes
# If "yes" than primary electrons will be generated
export ELECTRONS=yes

# Geometries
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_much.geo
    export SHIELDGEOM=shield_standard.geo
    export MVDGEOM= #mvd_standard.geo
    export STSGEOM=sts_standard.geo
#    export STSGEOM=sts_same_z.geo
    export MUCHGEOM=
    export RICHGEOM=rich_standard.geo
    export TRDGEOM=trd_standard.geo
    export TOFGEOM=tof_standard.geo
    export ECALGEOM=
    export FIELDMAP=field_electron_standard
    export MAGNETGEOM=magnet_electron_standard.geo

#number of events for each thread
export NEVENTS=3

for Y in 0; do
 for X in 0; do
  XXXX=00$Y$X
  echo batch $XXXX submitted
  export INFILE=/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
  export MCFILE=$DIR/mc.$XXXX.root
  export PARFILE=$DIR/param.$XXXX.root
  export RECOFILE=$DIR/reco.$XXXX.root
  export RECORICHFILE=$DIR/rich.reco.$XXXX.root
  export ELIDFILE=$DIR/elid.qa.$XXXX.root
  
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
