#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/home/andrey/cbm/trunk/build/

#Parameters for muon simulation
#Output directory for simulation files
export DIR=/home/d/andrey/std18_10mu

#If "yes" than 10 primary muons will be generated
export MUONS=yes

#If "yes" than UrQMD will be used as background
export URQMD=no

#If "yes" PLUTO particles will be embedded
export PLUTO=no

#MUCH geometry file name
export MUCHGEOM=much_standard_18.geo

#MUCH digitization file
export MUCHDIGI=$VMCWORKDIR/parameters/much/much_standard_18.digi.root

#TRD geometry file name
export TRDGEOM=

#number of events for each thread
export NEVENTS=1000

for Y in 0; do
 for X in 0; do
  XXXX=00$Y$X
  echo batch $XXXX submitted
  export INFILE=/home/d/events/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
  export PLUTOFILE=/u/andrey/cbm/much/pluto/omega/25gev/omega.$XXXX.root
  export MCFILE=$DIR/mc.$XXXX.root
  export PARFILE=$DIR/param.$XXXX.root
  export GLOBALRECOFILE=$DIR/global.reco.$XXXX.root
  export GLOBALHITSFILE=$DIR/global.hits.$XXXX.root
  export GLOBALTRACKSFILE=$DIR/global.tracks.$XXXX.root
  . ./much.sh
# bsub -q betch64 -J mc.$XXXX.run -o log/$XXXX.log -N -u andrey.lebedev@gsi.de sh much.sh
 done
done
export SCRIPT=no
