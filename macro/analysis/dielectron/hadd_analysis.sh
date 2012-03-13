#!/bin/sh

DIR=/lustre/cbm/user/ebelolap/aug11/mar12/25gev/100field/nomvd/phi

OUTPUTFILE=$DIR/analysis.all.root
INPUTFILES=$DIR/analysis.0*.root

hadd  -T -f $OUTPUTFILE $INPUTFILES
