#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MYBUILDDIR=/u/andrey/cbm/trunk/build_new/

#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -

#Output directory for simulation files
export DIR=/d/cbm02/andrey/test_muons

#Output directory for image files
export IMAGEDIR=./test_muons/

# CBM setup that will be used for simulation: "muon" or "electron"
export DETECTORSETUP=muon

# Global tracking type to be used
export TRACKINGTYPE=branch

#Simulation parameters
# number of embedded muons
export NMUONSPLUS=5
# number of embedded muons
export NMUONSMINUS=5
# number of embedded electrons
export NELECTRONS=5
# number of embedded positrons
export NPOSITRONS=5
# number of embedded pions
export NPIONSPLUS=5
# number of embedded pions
export NPIONSMINUS=5
# number of embedded particles from pluto. MAX = 10.
export NPLUTO=10
# If "yes" than UrQMD will be used as background
export URQMD=no
# If "yes" than primary muons will be generated
export MUONS=yes
# If "yes" than primary electrons will be generated
export ELECTRONS=no
# If "yes" than primary pions will be generated
export PIONS=no
# If "yes" PLUTO particles will be embedded
export PLUTO=no


if [ "$1" = "muon" ] ; then
    export DIR=/d/cbm02/andrey/test_muons
    export IMAGEDIR=./test_muons/
    export DETECTORSETUP=muon
    export URQMD=yes
    export MUONS=yes
    export ELECTRONS=no
    export PIONS=no
    export PLUTO=no
else

if [ "$1" = "electron" ] ; then
    export DIR=/d/cbm02/andrey/test_electrons
    export IMAGEDIR=./test_electrons/
    export DETECTORSETUP=electron
    export URQMD=yes
    export MUONS=no
    export ELECTRONS=yes
    export PIONS=no
    export PLUTO=no
fi
fi



# Geometries
if [ $DETECTORSETUP = "muon" ]; then
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_much.geo
    export SHIELDGEOM=shield_standard.geo
    export MVDGEOM=
    export STSGEOM=sts_standard.geo
    export STSDIGI=$VMCWORKDIR/parameters/sts/sts_standard.digi.par
    export MUCHGEOM=much_standard_2layers.geo
    export MUCHDIGI=$VMCWORKDIR/parameters/much/much_standard_2layers.digi.root
    export RICHGEOM=
    export TRDGEOM=
#trd_muon_setup_new.geo
    export TRDDIGI=
    export TOFGEOM=tof_standard.geo
    export ECALGEOM=
    export FIELDMAP=field_muon_standard
    export MAGNETGEOM=magnet_muon_standard.geo
else
    export CAVEGEOM=cave.geo
    export TARGETGEOM=target_au_250mu.geo
    export PIPEGEOM=pipe_much.geo
    export SHIELDGEOM=shield_standard.geo
    export MVDGEOM= #mvd_standard.geo
    export STSGEOM=sts_same_z.geo
    export STSDIGI=$VMCWORKDIR/parameters/sts/sts_same_z.digi.par
    export MUCHGEOM=
    export MUCHDIGI=
    export RICHGEOM=rich_standard.geo
    export TRDGEOM=trd_standard.geo
    export TRDDIGI=$VMCWORKDIR/parameters/trd/trd_standard.digi.par
    export TOFGEOM=tof_standard.geo
    export ECALGEOM=
    export FIELDMAP=field_electron_standard
    export MAGNETGEOM=magnet_electron_standard.geo
fi

#number of events for each thread
export NEVENTS=800

for Y in 0; do
 for X in 0; do
  XXXX=00$Y$X
  echo batch $XXXX submitted
  export INFILE=/d/cbm03/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.$XXXX.ftn14
  export MCFILE=$DIR/mc.$XXXX.root
  export PARFILE=$DIR/param.$XXXX.root
  export GLOBALRECOFILE=$DIR/global.reco.$XXXX.root
  export GLOBALHITSFILE=$DIR/global.hits.$XXXX.root
  export GLOBALTRACKSFILE=$DIR/global.tracks.$XXXX.root
  export RICHFILE=$DIR/rich.reco.$XXXX.root
  export ELIDFILE=$DIR/elid.qa.$XXXX.root
  
  COUNT=0
  while [ $COUNT -lt $NPLUTO ]; do
    echo PLUTO FILE COUNT IS: $COUNT
    export PLUTOFILE$COUNT=/home/d/andrey/pluto/Jpsi.000$COUNT.root
    let COUNT=COUNT+1    
  done 
  
  root -b -q "./global_sim.C($NEVENTS)"
  root -b -q "./global_reco.C($NEVENTS, \"all\")"
  #root -b -q "./global_reco.C($NEVENTS, \"hits\")"
  #root -b -q "./global_reco.C($NEVENTS, \"tracking\")"
  root -b -q "../rich/run/run_reco_rich.C($NEVENTS)"
  root -b -q "../rich/electronId/run_electrons_qa.C($NEVENTS)"
  
#  . ./sim.sh
# bsub -q betch64 -J mc.$XXXX.run -o log/$XXXX.log -N -u andrey.lebedev@gsi.de sh much.sh
 done
done
export SCRIPT=no



#FOR TRD TEST

#for X in 100 200 300 400 500 600 700 800 900 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000 30000 50000 70000 100000; do
#for X in 1000; do
#	export TRDHITERR=$X
#	export GLOBALHITSFILE=$DIR/global.hits.trd$TRDHITERR.0000.root
#	export GLOBALTRACKSFILE=$DIR/global.tracks.trd$TRDHITERR.0000.root
#	export GLOBALTRACKSIDEALFILE=$DIR/global.tracks.ideal.trd$TRDHITERR.0000.root
#	export PROPANAFILE=$DIR/propagation.ana.trd$TRDHITERR.0000.root
#	export IMAGEDIR=./test/trdstd_urqmd/trd$TRDHITERR/
#	rm -r $IMAGEDIR
#	mkdir $IMAGEDIR
#	root -b -q "./global_hits.C($NEVENTS)"
#	root -b -q "./global_tracking.C($NEVENTS)"
#	root -b -q "./global_reco_ideal.C($NEVENTS)"
#	root -b -q "./prop_ana.C($NEVENTS)"
#done

