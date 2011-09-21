#!/bin/sh

export SCRIPT=yes

. ./common.sh

export NEVENTS=$1

create_image_dir $2

set_default_electron_geometry

export STSHITPRODUCERTYPE=real
export TRACKINGTYPE=branch
export TRDGEOM=trd/trd_11a.geo
export TRDDIGI=$3
export TRDHITPRODUCERTYPE=$4

export MCFILE=$6/mc.0000.root
export PARFILE=$6/param.0000.root
export GLOBALRECOFILE=$6/global.reco.$5.0000.root
export GLOBALHITSFILE=$6/global.hits.$5.0000.root
export GLOBALTRACKSFILE=$6/global.tracks.$5.0000.root

#root -b -q "./global_reco.C($NEVENTS, \"all\")"
root -b -q "./global_reco.C($NEVENTS, \"hits\")"
root -b -q "./global_reco.C($NEVENTS, \"tracking\")"

export SCRIPT=no
