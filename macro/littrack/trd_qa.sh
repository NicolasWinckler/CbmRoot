#!/bin/sh

export SCRIPT=yes

. ./common.sh

export NEVENTS=$1

DIR=/data.local1/andrey/events/trd_v11a/events_$2
create_output_dir $DIR
IMAGEDIR=/data.local1/andrey/events/trd_v11a/results_$2
create_image_dir $IMAGEDIR

export DETECTORSETUP=electron

#     NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU E   PI  PLUTO USEUNIGEN
pars=(5    5    5   5   20   20   10     yes   no yes no  no    no)
set_simulation_parameters $pars

set_default_electron_geometry

export TRACKINGTYPE=branch
export INFILE=/data.local1/andrey/tests/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
export MCFILE=$DIR/mc.0000.root
export PARFILE=$DIR/param.0000.root
export GLOBALRECOFILE=$DIR/global.reco.0000.root
export GLOBALHITSFILE=$DIR/global.hits.0000.root
export GLOBALTRACKSFILE=$DIR/global.tracks.0000.root

export STSHITPRODUCERTYPE=real
export TRACKINGTYPE=branch
export TRDGEOM=trd/trd_v11a.geo
export TRDDIGI=$3
export TRDHITPRODUCERTYPE=$4

root -b -q "./global_sim.C($NEVENTS)"
root -b -q "./global_reco.C($NEVENTS, \"all\")"
#root -b -q "./global_reco.C($NEVENTS, \"hits\")"
#root -b -q "./global_reco.C($NEVENTS, \"tracking\")"


export SCRIPT=no
