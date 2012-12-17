#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MY_BUILD_DIR=/Users/slebedev/Development/cbm/trunk/build/

export RICH_GEO_TEST_RESULT_DIR=results/

#prototype or standard
export RICH_DETECTOR_TYPE=standard

# Geometries
export CAVE_GEOM=cave.geo
export TARGET_GEOM=target_au_250mu.geo
export PIPE_GEOM=pipe_standard.geo
export STS_GEOM=sts/sts_v12b.geo.root
export RICH_GEOM=rich/rich_v08a.geo
export FIELD_MAP=field_v12a
export MAGNET_GEOM=passive/magnet_v12a.geo
export FIELD_MAP_SCALE=1.0

#number of events for each thread
export NEVENTS=50000

for Z in 1 2; do
   
   export DIR=/Users/slebedev/Development/cbm/data/simulations/richgeotest/
   if [ $Z = "1" ] ; then
      export RICH_GEOM=rich/rich_v08a.geo
   fi
   if [ $Z = "2" ] ; then
      export RICH_GEOM=rich/rich_v08a_ed1.geo
   fi
   
   export RICH_GEO_TEST_RESULT_DIR=test_bel_results${Z}/
   mkdir -p ${RICH_GEO_TEST_RESULT_DIR}

   export MC_FILE=${DIR}/test.bel.mc.000${Z}.root
   export PAR_FILE=${DIR}/test.bel.param.000${Z}.root
   export RECO_FILE=${DIR}/test.bel.reco.000${Z}.root  
   
  # ./sim.sh
   xterm -hold -e ". ./sim.sh"&
 #  bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
   

done
export SCRIPT=no
