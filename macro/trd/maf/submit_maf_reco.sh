#!/bin/bash
CBMPATH=/data6/cbm/software/cbmroot_trunk_20140508/macro/trd/maf
# NEVENTS can be between 1 and 1000 - (limit is number of events in urqmd file)
NEVENTS=1000
#//NEVENTS=1
# NURQMD can be between 0 and 399 - (number of urqmd files)
NURQMD=399
#//NURQMD=0
for (( i = 0; i <= $NURQMD; i++ )) do
  qsub -q "long"  -d $CBMPATH/.. -e $CBMPATH/output -o $CBMPATH/output -v NEVENTS=$NEVENTS,NURQMD=$i "$CBMPATH/maf_run_reco.sh";
# qsub -q "short" -d $CBMPATH/.. -e $CBMPATH/output -o $CBMPATH/output -v NEVENTS=$NEVENTS,NURQMD=$i "$CBMPATH/maf_run_reco.sh";
  sleep 5
done
