#!/bin/sh

DIR=/lustre/cbm/user/ebelolap/aug11/25gev/100field/mvd/omega

OUTPUTFILE=$DIR/analysis.delta.all.root
INPUTFILES=$DIR/analysis.delta.0*.root

hadd -T -f $OUTPUTFILE $INPUTFILES
