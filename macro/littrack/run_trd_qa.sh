#!/bin/sh

. ./common.sh

export NEVENTS=500

create_output_dir /data.local1/andrey/events/trd_v11a

export DETECTORSETUP=electron

#     NMU+ NMU- NE- NE+ NPI+ NPI- NPLUTO URQMD MU E   PI  PLUTO USEUNIGEN
pars=(5    5    5   5   20   20   10     yes   no yes no  no    no)
set_simulation_parameters $pars

set_default_electron_geometry

export TRACKINGTYPE=branch

export $DIR=/data.local1/andrey/events/trd_v11a/
export INFILE=/data.local1/andrey/tests/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
export MCFILE=$DIR/mc.0000.root
export PARFILE=$DIR/param.0000.root

root -b -q "./global_sim.C($NEVENTS)"

. ./trd_qa.sh $NEVENTS clustering_trd_v11a_7.5mm_01cm2/ $VMCWORKDIR/parameters/trd/v11a/trd_v11a_7.5mm_01cm2.digi.par clustering clustering_trd_v11a_7.5mm_01cm2 $DIR
. ./trd_qa.sh $NEVENTS clustering_trd_v11a_7.5mm_02cm2/ $VMCWORKDIR/parameters/trd/v11a/trd_v11a_7.5mm_02cm2.digi.par clustering clustering_trd_v11a_7.5mm_02cm2 $DIR
. ./trd_qa.sh $NEVENTS clustering_trd_v11a_7.5mm_03cm2/ $VMCWORKDIR/parameters/trd/v11a/trd_v11a_7.5mm_03cm2.digi.par clustering clustering_trd_v11a_7.5mm_03cm2 $DIR
. ./trd_qa.sh $NEVENTS clustering_trd_v11a_7.5mm_04cm2/ $VMCWORKDIR/parameters/trd/v11a/trd_v11a_7.5mm_04cm2.digi.par clustering clustering_trd_v11a_7.5mm_04cm2 $DIR
. ./trd_qa.sh $NEVENTS clustering_trd_v11a_7.5mm_06cm2/ $VMCWORKDIR/parameters/trd/v11a/trd_v11a_7.5mm_06cm2.digi.par clustering clustering_trd_v11a_7.5mm_06cm2 $DIR
. ./trd_qa.sh $NEVENTS clustering_trd_v11a_7.5mm_10cm2/ $VMCWORKDIR/parameters/trd/v11a/trd_v11a_7.5mm_10cm2.digi.par clustering clustering_trd_v11a_7.5mm_10cm2 $DIR
. ./trd_qa.sh $NEVENTS clustering_trd_v11a_7.5mm_00.5cm2/ $VMCWORKDIR/parameters/trd/v11a/trd_v11a_7.5mm_00.5cm2.digi.par clustering clustering_trd_v11a_7.5mm_00.5cm2 $DIR

