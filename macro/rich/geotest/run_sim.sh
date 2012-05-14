#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/u/slebedev/JUL09/buildSC

export RICH_GEO_TEST_RESULT_DIR=results/

#prototype or standard
export RICH_DETECTOR_TYPE=prototype

# Geometries
	export CAVEGEOM=cave.geo
	export TARGETGEOM=target_au_025mu.geo
	export PIPEGEOM=pipe_standard.geo
	export STSGEOM=sts/sts_v11a.geo
	export RICHGEOM=rich/rich_v08a.geo
	export FIELDMAP=field_v10e
	export MAGNETGEOM=passive/magnet_v09e.geo
	export FIELDMAPSCALE=1.0

#number of events for each thread
export NEVENTS=1000

for Z in 0 ; do

   export DIR=/d/cbm06/user/slebedev/geotest
   export XXXX=000$Z
   export RICHGEOM=rich/rich_prototype_standard.geo
      
   export RICH_GEO_TEST_RESULT_DIR=results$XXXX/

   export MCFILE=$DIR/mc.$XXXX.root
   export PARFILE=$DIR/param.$XXXX.root
   export RECOFILE=$DIR/reco.$XXXX.root  
   
  . ./sim.sh
  # xterm -hold -e ". ./sim.sh"&
 #  bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
   

done
export SCRIPT=no
