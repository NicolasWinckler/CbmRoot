#!/bin/sh

echo $PATH
echo $LD_LIBRARY_PATH
which root

cd /u/andrey/cbm/trunk/build_lxir039/
. ./config.sh
cd -

. $VMCWORKDIR/macro/littrack/common.sh

export SCRIPT=yes

cd /data.local1/andrey/sts_qa_$5/
create_output_dir $3_$4_events_$5
create_image_dir $3_$4_results_$5
cd -

export DIR=/data.local1/andrey/sts_qa_$5/$3_$4_events_$5/
export IMAGEDIR=/data.local1/andrey/sts_qa_$5/$3_$4_results_$5/

export NEVENTS=$1

if [ "$5" = "muon" ] ; then
    export DETECTORSETUP=muon
#         NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU   E  PI PLUTO USEUNIGEN
    pars=(5    5    5   5   5    5    10     yes    yes no no no    no)
    set_simulation_parameters $pars
    set_default_muon_geometry
else

if [ "$5" = "electron" ] ; then
    export DETECTORSETUP=electron
#         NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU E   PI PLUTO USEUNIGEN
    pars=(5    5    5   5   5    5    10     yes   no yes no no    no)
    set_simulation_parameters $pars    
    set_default_electron_geometry

fi
fi

export STSGEOM=sts/$3
export STSDIGI=$VMCWORKDIR/parameters/sts/$4

export STSHITPRODUCERTYPE=real
export TRACKINGTYPE=branch
export TRDHITPRODUCERTYPE=smearing

set_default_file_names $DIR 0000

export INFILE=/data.local1/andrey/tests/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
    
root -b -q -l "$VMCWORKDIR/macro/littrack/global_sim.C($NEVENTS)"
#root -b -q "../global_reco.C($NEVENTS, \"all\")"
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"hits\")"
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"

export SCRIPT=no
