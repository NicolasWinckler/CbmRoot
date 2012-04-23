#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/u/slebedev/JUL09/buildSC

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

export LIT_RESULT_DIR=recoqa/


# Geometries
	export CAVEGEOM=cave.geo
	export TARGETGEOM=target_au_025mu.geo
	export PIPEGEOM=pipe_standard.geo
	export SHIELDGEOM=
	export MVDGEOM=
	#mvd/mvd_v08a.geo
	export STSGEOM=sts/sts_v11a.geo
	export MUCHGEOM=
	export RICHGEOM=rich/rich_v08a.geo
	export TRDGEOM=trd/trd_v10b.geo
	export TOFGEOM=tof/tof_v07a.geo
	export ECALGEOM=
	export FIELDMAP=field_v10e
	export MAGNETGEOM=passive/magnet_v09e.geo
	export FIELDMAPSCALE=1.0

#number of events for each thread
export NEVENTS=1000

for Z in 0 1 2 3 4; do
   export DIR=/d/cbm06/user/slebedev/rich
   export XXXX=000$Z
   
   export INFILE=/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
   export MCFILE=$DIR/mc.$XXXX.root
   export PARFILE=$DIR/param.$XXXX.root
   export RECOFILE=$DIR/reco.$XXXX.root   
   
   
    if [ $Z = "0" ] ; then
      export LIT_RESULT_DIR=recqa/rich_15/
      export RICHGEOM=rich/rich_compact_15.geo
   fi

   if [ $Z = "1" ] ; then
      export LIT_RESULT_DIR=recqa/rich_10/
      export RICHGEOM=rich/rich_compact_10.geo
   fi  

    if [ $Z = "2" ] ; then
      export LIT_RESULT_DIR=recqa/rich_5/
      export RICHGEOM=rich/rich_compact_5.geo
   fi
   
   if [ $Z = "3" ] ; then
      export LIT_RESULT_DIR=recqa/rich_minus1/
      export RICHGEOM=rich/rich_compact_minus1.geo
   fi
   
    if [ $Z = "4" ] ; then
      export LIT_RESULT_DIR=recqa/rich_v08a/
      export RICHGEOM=rich/rich_v08a.geo
   fi

   
 # . ./sim.sh
   xterm -hold -e ". ./sim.sh"&
   # bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
   
done
export SCRIPT=no
