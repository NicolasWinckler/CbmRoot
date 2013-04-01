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
export NEVENTS=30000

for Z in 0; do
   export DIR=/Users/slebedev/Development/cbm/data/simulations/richgeotest/
   
   export RICH_GEO_TEST_RESULT_DIR=results_tdr${Z}/
   mkdir -p ${RICH_GEO_TEST_RESULT_DIR}

   export MC_FILE=${DIR}/tdr.mc.000${Z}.root
   export PAR_FILE=${DIR}/tdr.param.000${Z}.root
   export RECO_FILE=${DIR}/tdr.reco.0001.root  
   
   ./sim.sh
  # xterm -hold -e ". ./sim.sh"&
 #  bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
   

done
export SCRIPT=no
