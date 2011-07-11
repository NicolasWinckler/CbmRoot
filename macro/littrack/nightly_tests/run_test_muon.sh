#!/bin/sh

echo $PATH
echo $LD_LIBRARY_PATH
which root

. $VMCWORKDIR/macro/littrack/common.sh

export SCRIPT=yes

create_output_dir events_muon/
create_image_dir results_muon/

export NEVENTS=10
export DETECTORSETUP=muon

#     NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU  E   PI  PLUTO USEUNIGEN
pars=(5    5    5   5   20   20   10     yes   yes no  no  no    yes)
set_simulation_parameters $pars    

set_default_muon_geometry

export TRACKINGTYPE=branch
export TRDHITPRODUCERTYPE=smearing

set_default_file_names $DIR 0000

export INFILE=$VMCWORKDIR/input/urqmd.auau.25gev.centr.root
    
root -b -q -l "$VMCWORKDIR/macro/littrack/global_sim.C($NEVENTS)"
#root -b -q "../global_reco.C($NEVENTS, \"all\")"
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"hits\")"
root -b -q -l "$VMCWORKDIR/macro/littrack/global_reco.C($NEVENTS, \"tracking\")"

export SCRIPT=no