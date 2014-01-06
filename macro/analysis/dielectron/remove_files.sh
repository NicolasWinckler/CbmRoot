#!/bin/bash

export DIR=/hera/cbm/users/slebedev/mc/dielectron/nov13/25gev/trd/1.0field/nomvd/

function remove_func {
   rm -r -f ${DIR}/${1}/log/*.log
   
   rm -r -f ${DIR}/${1}/analysis.*.root
   rm -r -f ${DIR}/${1}/FairRunInfo_analysis.*.root
   
  # rm -r -f ${DIR}/${1}/reco.*.root    
  # rm -r ${DIR}/${1}/FairRunInfo_reco.*.root
   
  # rm -r -f ${DIR}/${1}/litqa.*.root    
  # rm -r ${DIR}/${1}/FairRunInfo_litqa.*.root 
  # rm -r ${DIR}/${1}/reco.delta.0*.root
  # rm -r ${DIR}/${1}/mc.0*.root
}

remove_func phi
remove_func rho0
remove_func omegadalitz
remove_func omegaepem
#remove_func urqmd


