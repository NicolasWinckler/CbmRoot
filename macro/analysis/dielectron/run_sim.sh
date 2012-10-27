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

#Geometries
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
	export FIELDMAPSCALE=1.0

export FIELDDIR=100field
export MVDDIR=mvd
export MAINDIR=/lustre/cbm/user/ebelolap/aug11/sep12/${ENERGY}/${FIELDDIR}/${MVDDIR}
	
#If "yes" DELTA electrons will be embedded
export DELTA=no
export DELTAFILE=/lustre/cbm/user/ebelolap/aug11/sep12/${ENERGY}/${FIELDDIR}/deltasource/mc.delta.root

#number of events for each thread
export NEVENTS=1000

export PIONMISIDENTIFICATIONLEVEL=-1.0
export USEMCMOMENTUM=yes

for Z in 1 2 3 4; do
    if [ $Z = "0" ] ; then
        export PLUTOPARTICLE=
        export DIR=
        export PLUTO=no
    fi
	if [ $Z = "1" ] ; then 
		export PLUTOPARTICLE=rho0 
		export DIR=${MAINDIR}/rho0
	fi	
	if [ $Z = "2" ] ; then 
		export PLUTOPARTICLE=omegaepem
		export DIR=${MAINDIR}/omega
	fi	
	if [ $Z = "3" ] ; then 
		export PLUTOPARTICLE=omegadalitz
		export DIR=${MAINDIR}/omegadalitz
	fi	
	if [ $Z = "4" ] ; then 
		export PLUTOPARTICLE=phi
		export DIR=${MAINDIR}/phi
	fi
    if [ $Z = "5" ] ; then 
		export PLUTOPARTICLE=pi0 
		export DIR=
	fi	

 #for K in 0; do
 # for Y in 0 ; do
  # for X in 1 ; do

 for K in 0 1; do
	for Y in 0 1 2 3 4 5 6 7 8 9; do
    for X in 0 1 2 3 4 5 6 7 8 9 ; do

	  export XXXX=0${K}${Y}${X}
	  #echo batch ${XXXX} submitted
	  export INFILE=/d/cbm03/urqmd/auau/${ENERGY}/centr/urqmd.auau.${ENERGY}.centr.${XXXX}.ftn14
	  export MCFILE=${DIR}/mc.${XXXX}.root
	  export PARFILE=${DIR}/param.${XXXX}.root
      export RECOFILE=${DIR}/reco.${XXXX}.root
      export RECOQAFILE=${DIR}/reco.litqa.${XXXX}.root
    #export DILEPANALYSISFILE=${DIR}/analysis.pi_misid_${PIONMISIDENTIFICATIONLEVEL}.${XXXX}.root
      export DILEPANALYSISFILE=${DIR}/analysis.mc_momentum.${XXXX}.root 
    # export DILEPANALYSISFILE=${DIR}/analysis.${XXXX}.root
 
 
	  if [ $Z = "1" ] ; then 
		export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/${ENERGY}/rho0/epem/pluto.auau.${ENERGY}.rho0.epem.${XXXX}.root
	  fi	
	  if [ $Z = "2" ] ; then 
	  	export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/${ENERGY}/omega/epem/pluto.auau.${ENERGY}.omega.epem.${XXXX}.root
	  fi	
	  if [ $Z = "3" ] ; then 
  		  export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/${ENERGY}/omega/pi0epem/pluto.auau.${ENERGY}.omega.pi0epem.${XXXX}.root
  	  fi	
	  if [ $Z = "4" ] ; then 
		  export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/${ENERGY}/phi/epem/pluto.auau.${ENERGY}.phi.epem.${XXXX}.root
	  fi	  
 	  if [ $Z = "5" ] ; then 
		  export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/${ENERGY}/pi0/gammaepem/pluto.auau.${ENERGY}.pi0.gammaepem.10k.${XXXX}.root
	  fi	  
      #	. ./sim.sh
	 	# xterm -hold -e ". ./sim.sh $number"&
       bsub -q batch -J mc.${Z}.${XXXX}.run -o ${DIR}/log/${XXXX}.log -N sh ./sim.sh
     	
	  done
	 done
	done
done
export SCRIPT=no
