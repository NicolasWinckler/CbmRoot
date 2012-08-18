#!/bin/bash
# NEVENTS can be between 1 and 500
NEVENTS=500
# NURQMD can be between 0 and 199
NURQMD=0
#for i in {0..${NURQMD}}
#do
#root -l -b -q run_sim_pi0.C'('${NEVENTS},${i}')'
#done
for i in {0..${NURQMD}}
do
root -l -b -q run_reco_pi0.C'('${NEVENTS},${i}')'
done