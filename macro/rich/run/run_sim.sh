#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/u/slebedev/JUL09/build64

#Output directory for simulation files
export DIR=/d/cbm02/slebedev/rich/JUL09/rho0

#Output directory for image files
export IMAGEDIR=/d/cbm02/slebedev/rich/JUL09/rho0/images/

export TRACKINGTYPE=branch

#Simulation parameters
# number of embedded electrons
export NELECTRONS=5
# number of embedded positrons
export NPOSITRONS=5
# If "yes" than UrQMD will be used as background
export URQMD=yes
# If "yes" than primary electrons will be generated
export ELECTRONS=no
# If "yes" PLUTO particles will be embedded
export PLUTO=yes

# Geometries
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_025mu.geo
    export PIPEGEOM=pipe_standard.geo
    export SHIELDGEOM=shield_standard.geo
    export MVDGEOM=mvd_2maps_2008.geo
    export STSGEOM=sts_standard.geo
#    export STSGEOM=sts_same_z.geo
    export MUCHGEOM=
    export RICHGEOM=rich_standard.geo
    export TRDGEOM= #trd_standard.geo
    export TOFGEOM=tof_standard.geo
    export ECALGEOM=
    export FIELDMAP=field_electron_standard
    export MAGNETGEOM=magnet_electron_standard.geo

#number of events for each thread
export NEVENTS=500

for Z in 1; do
	if [ $Z = "1" ] ; then 
		export PLUTOPARTICLE=rho0 
		export DIR=/lustre/cbm/user/slebedev/rho0
	fi	
	if [ $Z = "2" ] ; then 
		export PLUTOPARTICLE=omegaepem
		export DIR=/lustre/cbm/user/slebedev/omega/epem
	fi	
	if [ $Z = "3" ] ; then 
		export PLUTOPARTICLE=omegadalitz
		export DIR=/lustre/cbm/user/slebedev/omega/dalitz
	fi	
	if [ $Z = "4" ] ; then 
		export PLUTOPARTICLE=phi
		export DIR=/lustre/cbm/user/slebedev/phi
	fi
	
	for Y in 0; do
	 for X in 0; do
	  XXXX=00$Y$X
	  echo batch $XXXX submitted
	  export INFILE=/d/cbm03/urqmd/auau/10gev/centr/urqmd.AuAu.10gev.centr.$XXXX.ftn14
	  export MCFILE=$DIR/mc.$XXXX.root
	  export PARFILE=$DIR/param.$XXXX.root
	  export RECOFILE=$DIR/reco.$XXXX.root
	  export RECORICHFILE=$DIR/rich.reco.$XXXX.root
	  export ELIDFILE=$DIR/elid.qa.$XXXX.root
	  export DILEPANALYSISFILE=$DIR/analysis.dilep.$XXXX.root
	  
	  if [ $Z = "1" ] ; then 
		  export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/8gev/rho0/epem/pluto.auau.8gev.rho0.epem.10k.$XXXX.root
	  fi	
	  if [ $Z = "2" ] ; then 
	  	export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/10gev/omega/epem/pluto.auau.10gev.omega.epem.10k.$XXXX.root
	  fi	
	  if [ $Z = "3" ] ; then 
  		  export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/10gev/omega/pi0epem/pluto.auau.10gev.omega.pi0epem.10k.$XXXX.root
  	  fi	
	  if [ $Z = "4" ] ; then 
		  export PLUTOFILE=/d/cbm05/galatyuk/pluto/auau/10gev/phi/epem/pluto.auau.10gev.phi.epem.10k.$XXXX.root
	  fi	  

	  . ./sim.sh
	 #xterm -hold -e ". ./sim.sh $number"&
	  #bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
	 done
	done
done
export SCRIPT=no
