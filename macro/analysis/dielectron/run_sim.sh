#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/u/ebelolap/OCT10/build64

#Output directory for simulation files
#export DIR=/lustre/cbm/user/ebelolap/oct10/rho0/10gev/100_field

#Output directory for image files
#export IMAGEDIR=/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/images/

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

#If "yes" DELTA electrons will be embedded
export DELTA=yes
#---------------------------------------------------
# Geometries
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_025mu.geo
    export PIPEGEOM=pipe_standard.geo
    #pipe_standard_tobias.geo
    #pipe_standard.geo
    export SHIELDGEOM=
   
    export MVDGEOM=mvd_2maps_2008.geo
    #3maps-5-10-15cm_active-MimoSIS-Malice_abs-Cu-1cm_noFPC.geo
    #mvd_2maps_2008.geo
    export STSGEOM=sts_Jan2011.geo
    export MUCHGEOM=
    export RICHGEOM=rich_standard.geo
    export TRDGEOM=trd_standard_dec10.geo
    export TOFGEOM=tof_standard.geo
    export ECALGEOM=
    export FIELDMAP=field_electron_standard
    export MAGNETGEOM=magnet_electron_standard.geo
	export FIELDMAPSCALE=0.7
#----------------------------------------------------	
#number of events for each thread
export NEVENTS=700

for Z in 1; do
    
    if [ $Z = "0" ] ; then
        export PLUTOPARTICLE=
        export DIR=/lustre/cbm/user/ebelolap/oct10/urqmd_only/25gev/70_field
        export PLUTO=no
    fi
	if [ $Z = "1" ] ; then 
		export PLUTOPARTICLE=rho0 
		export DIR=/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/70_field/delta
	fi	
	if [ $Z = "2" ] ; then 
		export PLUTOPARTICLE=omegaepem
		export DIR=/lustre/cbm/user/ebelolap/oct10/urqmd_omega/25gev/70_field/delta
	fi	
	if [ $Z = "3" ] ; then 
		export PLUTOPARTICLE=omegadalitz
		export DIR=/lustre/cbm/user/ebelolap/oct10/urqmd_omega_dalitz/25gev/70_field/delta
	fi	
	if [ $Z = "4" ] ; then 
		export PLUTOPARTICLE=phi
		export DIR=/lustre/cbm/user/ebelolap/oct10/urqmd_phi/25gev/70_field/delta
	fi
        if [ $Z = "5" ] ; then 
		export PLUTOPARTICLE=pi0 
		export DIR=/lustre/cbm/user/ebelolap/oct10/urqmd_pi0/25gev/70_field/real
	fi	

#    for K in 0 1; do
#   for Y in 0 ; do
#    for X in 1 ; do

   for K in 0; do
	 for Y in 0 1 2 3 4 5 6 7 8 9 ; do
      for X in 0 1 2 3 4 5 6 7 8 9 ; do

	  export XXXX=0$K$Y$X
	  echo batch $XXXX submitted
	  export INFILE=/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
	  export MCFILE=$DIR/mc.$XXXX.root
	  export PARFILE=$DIR/param.$XXXX.root
      export RECOFILE=$DIR/reco.nodelta.all.ideal.$XXXX.root
 	  export DELTAFILE=/lustre/cbm/user/ebelolap/oct10/delta/25gev/70_field/mc.delta.0000.root

	  export RECORICHFILE=$DIR/rich.reco.$XXXX.root
	  export ELIDFILE=$DIR/elid.qa.$XXXX.root
      export RECOQAFILE=$DIR/litreco.qa.$XXXX.root
	  export DILEPANALYSISFILE=$DIR/mytask.analysis.$XXXX.root

 	 
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
    #      . ./sim.sh
	 #xterm -hold -e ". ./sim.sh $number"&
       bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
	  done
	 done
	done
done
export SCRIPT=no
