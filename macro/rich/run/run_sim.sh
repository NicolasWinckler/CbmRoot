=#!/bin/sh

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
export TRD_GEOM=trd/trd_v13c.root
#export TRD_GEOM=
export TOF_GEOM=tof/tof_v07a.geo
#export TOF_GEOM=
export FIELD_MAP=field_v12a
export MAGNET_GEOM=passive/magnet_v12a.geo
export FIELD_MAP_SCALE=1.0

#number of events for each thread
export NEVENTS=100

for Z in 0; do
   export NOF_NOISE_HITS_IN_RICH=220
   export RICH_COLLECTION_EFF=1.0
   export SIGMA_ERROR_RICH=0.06
   export CROSSTALK_RICH=0.02

   export DIR=/Users/slebedev/Development/cbm/data/simulations/
   export LIT_RESULT_DIR=recqa_trdtest_${Z}/
   mkdir -p ${LIT_RESULT_DIR}
   
   export IN_FILE=/Users/slebedev/Development/cbm/data/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0004.ftn14
   export MC_FILE=${DIR}/25gev.mc.trdtest.0000.root
   export PAR_FILE=$DIR/25gev.param.trdtest.0000.root
   export RECO_FILE=$DIR/25gev.reco.trdtest.0000.root
   
  # export IN_FILE=/Users/slebedev/Development/cbm/data/urqmd/auau/8gev/centr/urqmd.auau.8gev.centr.000${Z}.ftn14
  # export MC_FILE=${DIR}/8gev.mc.000${Z}.root
  # export PAR_FILE=$DIR/8gev.param.000${Z}.root
  # export RECO_FILE=$DIR/8gev.reco.000${Z}.root
   
  . ./sim.sh
  
  # xterm -hold -e ". ./sim.sh"&
   # bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
   
done
export SCRIPT=no
