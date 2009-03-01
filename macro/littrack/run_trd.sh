#!/bin/sh
export ELECTRONS=no
export URQMD=yes
export NEVENTS=3
export GEOTYPE=monolithic
export DETECTOR=trd
export SCRIPT=yes
export MYBUILDDIR=/home/andrey/cbm/svnmar09/build/
for Y in 0; do
 for X in 0; do
  XXXX=00$Y$X
  echo batch $XXXX submitted
  export INFILE=/home/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
#  export PLUTOFILE=/u/andrey/cbm/much/pluto/omega/25gev/omega.$XXXX.root
  export DIR=/home/d/andrey/events/trd/monolithic/10e/e
  export MCFILE=$DIR/mc.$XXXX.root
  export PARFILE=$DIR/param.$XXXX.root
  export STSRECOFILE=$DIR/sts.reco.$XXXX.root
  export TRDHITSFILE=$DIR/trd.hits.$XXXX.root
  export TRDTRACKSFILE=$DIR/trd.tracks.$XXXX.root
  . ./trd.sh
  #bsub -q betch64 -J mc.$XXXX.run -o log/$XXXX.log -N -u andrey.lebedev@gsi.de sh trd.sh
 done
done
export SCRIPT=no