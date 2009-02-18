#!/bin/sh
for Y in 0; do
 for X in 0; do
  XXXX=00$Y$X
  echo batch $XXXX submitted
  export INFILE=/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
  export PLUTOFILE=/u/andrey/cbm/much/pluto/omega/25gev/omega.$XXXX.root
  export OUTFILE=/d/cbm02/andrey/batch/omega_urqmd/mc.$XXXX.root
  export PARFILE=/d/cbm02/andrey/batch/omega_urqmd/param.mc.$XXXX.root
  #script.sh
  bsub -q betch64 -J mc.$XXXX.run -o log/mc.$XXXX.log -N -u andrey.lebedev@gsi.de sh script.sh
 done
done