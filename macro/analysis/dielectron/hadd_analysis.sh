#!/bin/sh

DIR=/lustre/cbm/user/ebelolap/aug11/aug12/25gev/100field/mvd/omega

OUTPUTFILE=$DIR/analysis.all.root
INPUTFILES=$DIR/analysis.0*.root

hadd  -T -f $OUTPUTFILE $INPUTFILES
