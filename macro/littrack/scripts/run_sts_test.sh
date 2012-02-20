#!/bin/sh

. ./common.sh

export SCRIPT=yes
#export DIRPREFIX=/d/cbm02/andrey/electron/std_10e_urqmd_no_mvd
#export IMAGEDIRPREFIX=/u/andrey/cbm/trunk/cbmroot/macro/littrack/sts_test/electron
export DIRPREFIX=/d/cbm02/andrey/muon/std_10mu_urqmd
export IMAGEDIRPREFIX=/u/andrey/cbm/trunk/cbmroot/macro/littrack/sts_test/muon
export MYBUILDDIR=/u/andrey/cbm/trunk/build_lxir039/
    
cd $MYBUILDDIR
. ./config.sh
cd -

export DETECTORSETUP=muon
export NEVENTS=1000
export TRACKINGTYPE=branch

#set_default_electron_geometry
set_default_muon_geometry

for X in 0 2 4 6 8 12; do

    export DIGI=sts$X\deg
    
    if [ $X = 0 ] ; then
        export STSDIGI=$VMCWORKDIR/parameters/sts/sts_standard.digi.par
    else
        export STSDIGI=$VMCWORKDIR/parameters/sts/sts_standard_$X\_deg.digi.par
    fi

    export IMAGEDIR=$IMAGEDIRPREFIX/$DIGI/
    rm -r $IMAGEDIR
    mkdir $IMAGEDIR

    export MCFILE=$DIRPREFIX/mc.0000.root
    export PARFILE=$DIRPREFIX/param.0000.root
    export GLOBALRECOFILE=$DIRPREFIX/global.reco.$DIGI.0000.root
    export GLOBALHITSFILE=$DIRPREFIX/global.hits.$DIGI.0000.root
    export GLOBALTRACKSFILE=$DIRPREFIX/global.tracks.$DIGI.0000.root

    taskset -c 0 root -b -q "./global_reco.C($NEVENTS, \"all\")"
    #root -b -q "./global_reco.C($NEVENTS, \"hits\")"
    #root -b -q "./global_reco.C($NEVENTS, \"tracking\")"

done

