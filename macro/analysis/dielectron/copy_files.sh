#####!/bin/sh

dirFrom=/hera/cbm/users/slebedev/mc/dielectron/apr13_2/8gev/notrd/
dirTo=/u/slebedev/Baykal/copytemp/8gev/notrd/

filename=analysis.auau.8gev.centr.all.root

function copy_func() {
   mkdir -p ${dirTo}/${2}/${1}/
   cp ${dirFrom}/${2}/${1}/${filename} ${dirTo}/${2}/${1}/
}

function copy_all() {
   copy_func rho0 ${1}
  # copy_func omegaepem ${1}
  # copy_func omegadalitz ${1}
  # copy_func phi ${1}
  # copy_func urqmd ${1}
}

copy_all 1.0field/nomvd
#copy_all 0.7field/mvd4
#copy_all 0.7field/nomvd
#copy_all 1.0field/mvd
#copy_all 1.0field/mvd4
#copy_all 1.0field/nomvd

