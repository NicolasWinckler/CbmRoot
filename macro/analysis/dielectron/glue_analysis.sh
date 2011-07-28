#!/bin/sh

DIR=/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/70_field/delta

OUTPUTFILE=$DIR/mytask.analysis.delta.all.root
INPUTFILES=$DIR/mytask.analysis.delta.0*.root

hadd -T -f $OUTPUTFILE $INPUTFILES
