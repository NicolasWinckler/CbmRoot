#!/bin/sh
cd /u/cbmgast6/aivanov/cbm_7h1704160209/build64
. ./config.sh
cd -
root -b <<EOF
.x much_sim.C
.q
EOF
