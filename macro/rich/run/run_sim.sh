#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MY_BUILD_DIR=/Users/slebedev/Development/cbm/trunk/build

export TRACKINGTYPE=branch

#Simulation parameters
#--------------------------------------------------
# number of embedded electrons
export NELECTRONS=5

# number of embedded positrons
export NPOSITRONS=5

# If "yes" than UrQMD will be used as background
export URQMD=yes

# If "yes" than primary electrons will be generated
export ELECTRONS=yes

# If "yes" PLUTO particles will be embedded
export PLUTO=no

# Geometries
export CAVE_GEOM=cave.geo
export TARGET_GEOM=target_au_250mu.geo
export PIPE_GEOM=pipe_standard.geo
export MVD_GEOM=
export STS_GEOM=sts/sts_v12b.geo.root
export RICH_GEOM=rich/rich_v08a.geo
export TRD_GEOM=trd/trd_v10b.geo
export TOF_GEOM=tof/tof_v07a.geo
export FIELD_MAP=field_v12a
export MAGNET_GEOM=passive/magnet_v12a.geo
export FIELD_MAP_SCALE=1.0

#number of events for each thread
export NEVENTS=1000

for Z in 0.2 0.5 0.7 1.0 1.5 2.0; do
   export DIR=/Users/slebedev/Development/cbm/data/simulations/richreco/
   export LIT_RESULT_DIR=recqa${Z}/
   mkdir -p ${LIT_RESULT_DIR}
   export FIELD_MAP_SCALE=${Z}

   export IN_FILE=/Users/slebedev/Development/cbm/data/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
   export MC_FILE=${DIR}/mc.000${Z}.root
   export PAR_FILE=$DIR/param.000${Z}.root
   export RECO_FILE=$DIR/reco.000${Z}.root

  #. ./sim.sh
   xterm -hold -e ". ./sim.sh"&
   # bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
   
done
export SCRIPT=no
