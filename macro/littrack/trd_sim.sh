#!/bin/sh

. ./common.sh

default_init

export NEVENTS=$1

create_image_dir $2

export TRACKINGTYPE=branch
export TRDDIGI=$3
export TRDHITPRODUCERTYPE=$4

export GLOBALRECOFILE=$DIR/global.reco.$5.0000.root
export GLOBALHITSFILE=$DIR/global.hits.$5.0000.root
export GLOBALTRACKSFILE=$DIR/global.tracks.$5.0000.root

#root -b -q "./global_reco.C($NEVENTS, \"all\")"
root -b -q "./global_reco.C($NEVENTS, \"hits\")"
root -b -q "./global_reco.C($NEVENTS, \"tracking\")"

export SCRIPT=no
