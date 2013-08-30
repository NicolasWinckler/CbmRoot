#!/bin/sh

export DIR=/hera/cbm/users/slebedev/mc/dielectron/apr13_2/8gev/notrd/1.0field/nomvd/

function remove_func() {
  # rm -r -f ${DIR}/${1}/log/*.log
   rm -r -f ${DIR}/${1}/analysis.*.root
   rm -r -f ${DIR}/${1}/FairRunInfo_analysis.*.root
 #  rm -r ${DIR}/${1}/reco.*.root    
 #  rm -r ${DIR}/${1}/analysis.delta.0*.root
  # rm -r ${DIR}/${1}/reco.delta.0*.root
  # rm -r ${DIR}/${1}/mc.0*.root
}

#remove_func phi
remove_func rho0
#remove_func omegadalitz
#remove_func omegaepem
#remove_func urqmd


