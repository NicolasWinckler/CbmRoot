#!/bin/sh

export DIR=/lustre/cbm/user/ebelolap/aug11/mar12/25gev/100field/mvd

function remove_func() {
   rm -r ${DIR}/${1}/analysis.0*.root
   rm -r ${DIR}/${1}/reco.0*.root    
   rm -r ${DIR}/${1}/analysis.delta.0*.root
   rm -r ${DIR}/${1}/reco.delta.0*.root
   rm -r ${DIR}/${1}/mc.0*.root
}

remove_func phi
remove_func rho0
remove_func omegadalitz
remove_func omega


