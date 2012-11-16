#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/Users/slebedev/Development/cbm/trunk/build

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

export LIT_RESULT_DIR=recqa/


# Geometries
	export CAVEGEOM=cave.geo
	export TARGETGEOM=target_au_250mu.geo
	export PIPEGEOM=pipe_standard.geo
	export MVDGEOM=
	#mvd/mvd_v08a.geo
	export STSGEOM=sts/sts_v12b.geo.root
	export RICHGEOM=rich/rich_v08a.geo
	export TRDGEOM=trd/trd_v10b.geo
	export TOFGEOM=tof/tof_v07a.geo
	export ECALGEOM=
	export FIELDMAP=field_v12a
	export MAGNETGEOM=passive/magnet_v12a.geo
	export FIELDMAPSCALE=1.0

#number of events for each thread
export NEVENTS=10

for Z in 0; do
   export DIR=/Users/slebedev/Development/cbm/data/simulations/
   export XXXX=000$Z
   
   export INFILE=/Users/slebedev/Development/cbm/data/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
   export MCFILE=$DIR/mc.$XXXX.root
   export PARFILE=$DIR/param.$XXXX.root
   export RECOFILE=$DIR/reco.$XXXX.root

   
  . ./sim.sh
  # xterm -hold -e ". ./sim.sh"&
   # bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
   
done
export SCRIPT=no
