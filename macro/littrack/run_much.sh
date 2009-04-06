#!/bin/sh
export PLUTO=no
export MUONS=yes
export URQMD=no
export NEVENTS=1000
export GEOTYPE=new_much_standard
export SCRIPT=yes
export DETECTOR=much
export MYBUILDDIR=/home/andrey/cbm/svnapr09/build/
for Y in 0; do
 for X in 0; do
  XXXX=00$Y$X
  echo batch $XXXX submitted
  export INFILE=/home/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
#  export PLUTOFILE=/u/andrey/cbm/much/pluto/omega/25gev/omega.$XXXX.root
  export DIR=/home/d/andrey/test/mu
#events/strawmuch/standard/10mu/mu
  export MCFILE=$DIR/mc.$XXXX.root
  export PARFILE=$DIR/param.$XXXX.root
  export STSRECOFILE=$DIR/sts.reco.$XXXX.root
  export MUCHHITSFILE=$DIR/much.hits.$XXXX.root
  export MUCHTRACKSFILE=$DIR/much.tracks.$XXXX.root
  . ./much.sh
  #bsub -q betch64 -J mc.$XXXX.run -o log/$XXXX.log -N -u andrey.lebedev@gsi.de sh trd.sh
 done
done
export SCRIPT=no
export MUCH=no