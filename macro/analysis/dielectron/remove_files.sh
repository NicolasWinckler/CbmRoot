#!/bin/sh

export DIR=/lustre/cbm/user/ebelolap/aug11/sep12/25gev/70field/nomvd/

function remove_func() {
   rm -r ${DIR}/${1}/log/*.log
   rm -r ${DIR}/${1}/analysis.*.root
   rm -r ${DIR}/${1}/FairRunInfo_analysis.*.root
#   rm -r ${DIR}/${1}/reco.0*.root    
 #  rm -r ${DIR}/${1}/analysis.delta.0*.root
  # rm -r ${DIR}/${1}/reco.delta.0*.root
  # rm -r ${DIR}/${1}/mc.0*.root
}

remove_func phi
remove_func rho0
remove_func omegadalitz
remove_func omega


