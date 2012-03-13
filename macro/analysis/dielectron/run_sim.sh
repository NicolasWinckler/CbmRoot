#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/u/slebedev/JUL09/build64

export TRACKINGTYPE=branch

#Simulation parameters
#--------------------------------------------------
# number of embedded electrons
export NELECTRONS=50

# number of embedded positrons
export NPOSITRONS=50

# If "yes" than UrQMD will be used as background
export URQMD=yes

# If "yes" than primary electrons will be generated
export ELECTRONS=no

# If "yes" PLUTO particles will be embedded
export PLUTO=yes


 	  
#Collision energy: 25gev or 10gev -> set proper weight into analysis
export ENERGY=25gev
#---------------------------------------------------

# Geometries
	export CAVEGEOM=cave.geo
	export TARGETGEOM=target_au_025mu.geo
	export PIPEGEOM=pipe_standard.geo
	export SHIELDGEOM=
	export MVDGEOM=mvd/mvd_v08a.geo
	export STSGEOM=sts/sts_v11a.geo
	export MUCHGEOM=
	export RICHGEOM=rich/rich_v08a.geo
	export TRDGEOM=trd/trd_v10b.geo
	export TOFGEOM=tof/tof_v07a.geo
	export ECALGEOM=
	export FIELDMAP=field_v10e
	export MAGNETGEOM=passive/magnet_v09e.geo
	export FIELDMAPSCALE=0.7

export FIELDDIR=70field
export MVDDIR=mvd
	
	
#If "yes" DELTA electrons will be embedded
export DELTA=yes
export DELTAFILE=/lustre/cbm/user/ebelolap/aug11/mar12/25gev/$FIELDDIR/deltasource/mc.delta.root

#number of events for each thread
export NEVENTS=1000

C=0

for Z in 1 2 3 4; do
    if [ $Z = "0" ] ; then
        export PLUTOPARTICLE=
        export DIR=
        export PLUTO=no
    fi
	if [ $Z = "1" ] ; then 
		export PLUTOPARTICLE=rho0 
		export DIR=/lustre/cbm/user/ebelolap/aug11/mar12/25gev/$FIELDDIR/$MVDDIR/rho0
	fi	
	if [ $Z = "2" ] ; then 
		export PLUTOPARTICLE=omegaepem
		export DIR=/lustre/cbm/user/ebelolap/aug11/mar12/25gev/$FIELDDIR/$MVDDIR/omega
	fi	
	if [ $Z = "3" ] ; then 
		export PLUTOPARTICLE=omegadalitz
		export DIR=/lustre/cbm/user/ebelolap/aug11/mar12/25gev/$FIELDDIR/$MVDDIR/omegadalitz
	fi	
	if [ $Z = "4" ] ; then 
		export PLUTOPARTICLE=phi
		export DIR=/lustre/cbm/user/ebelolap/aug11/mar12/25gev/$FIELDDIR/$MVDDIR/phi
	fi
    if [ $Z = "5" ] ; then 
		export PLUTOPARTICLE=pi0 
		export DIR=
	fi	

  # for K in 0; do
   #for Y in 0 ; do
    #for X in 1 ; do

  for K in 0 1; do
	for Y in 0 1 2 3 4 5 6 7 8 9; do
    for X in 0 1 2 3 4 5 6 7 8 9 ; do

	  export XXXX=0$K$Y$X
	  #echo batch $XXXX submitted
	  export INFILE=/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
	  export MCFILE=$DIR/mc.$XXXX.root
	  export PARFILE=$DIR/param.$XXXX.root
      export RECOFILE=$DIR/reco.delta.$XXXX.root
      export RECOQAFILE=$DIR/reco.litqa.$XXXX.root
	  export DILEPANALYSISFILE=$DIR/analysis.delta.$XXXX.root

	  if [ $Z = "1" ] ; then 
		export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/25gev/rho0/epem/pluto.auau.25gev.rho0.epem.$XXXX.root
	  fi	
	  if [ $Z = "2" ] ; then 
	  	export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/25gev/omega/epem/pluto.auau.25gev.omega.epem.$XXXX.root
	  fi	
	  if [ $Z = "3" ] ; then 
  		  export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/25gev/omega/pi0epem/pluto.auau.25gev.omega.pi0epem.$XXXX.root
  	  fi	
	  if [ $Z = "4" ] ; then 
		  export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/25gev/phi/epem/pluto.auau.25gev.phi.epem.$XXXX.root
	  fi	  
 	  if [ $Z = "5" ] ; then 
		  export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/25gev/pi0/gammaepem/pluto.auau.25gev.pi0.gammaepem.10k.$XXXX.root
	  fi	  
      # 	. ./sim.sh
	 	# xterm -hold -e ". ./sim.sh $number"&
     	 bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
     	
     	#FILESIZE=$(stat -c%s "$MCFILE")
        #FILESIZERECO=$(stat -c%s "$RECOFILE")        
     	#if [ ! -e $MCFILE ] || [ "$FILESIZE" -le 1300000000 ] || [ ! -e $RECOFILE ] || [ "$FILESIZERECO" -le 1200000000 ]; then
        #if [ ! -e $MCFILE ]; then                        
        #        bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
        #        (( C += 1 ))
        #        echo $C
     	#fi
	  done
	 done
	done
        echo $COUNT
done
export SCRIPT=no
