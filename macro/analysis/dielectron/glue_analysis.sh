#!/bin/sh

DIR=/lustre/cbm/user/ebelolap/oct10/urqmd_omega/25gev/100_field/real
OUTPUTFILE=$DIR/mytask.analysis.all_temp.root
INPUTFILES=$DIR/mytask.analysis.0*.root

hadd -T -f $OUTPUTFILE $INPUTFILES
