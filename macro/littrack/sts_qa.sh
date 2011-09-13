#!/bin/sh

echo $PATH
echo $LD_LIBRARY_PATH
which root

cd /u/andrey/cbm/trunk/build_lxir039/
. ./config.sh
cd -

. $VMCWORKDIR/macro/littrack/common.sh

export SCRIPT=yes

cd /data.local1/andrey/sts_qa_muon/
create_output_dir $3_$4_events_muon
create_image_dir $3_$4_results_muon
cd -

export DIR=/data.local1/andrey/sts_qa_muon/$3_$4_events_muon/
export IMAGEDIR=/data.local1/andrey/sts_qa_muon/$3_$4_results_muon/

export NEVENTS=$1
export DETECTORSETUP=muon

#     NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU  E  PI  PLUTO USEUNIGEN
pars=(5    5    5   5   20   20   10     yes   yes no no  no    no)
set_simulation_parameters $pars    

set_default_muon_geometry

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
