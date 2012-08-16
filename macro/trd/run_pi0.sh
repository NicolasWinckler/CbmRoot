#!/bin/bash
# i can be between 0 and 199
NEVENTS=100
#for i in {0..1}
#do
#root -l -b -q run_sim_pi0.C'('${NEVENTS},${i}')'
#done
for i in {0..0}
do
root -l -b -q run_reco_pi0.C'('${NEVENTS},${i}')'
done