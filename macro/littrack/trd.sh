#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -
root -b <<EOF
.x trd_sim.C($NEVENTS)
.q
EOF

root -b <<EOF
.x sts_reco.C($NEVENTS)
.q
EOF

root -b <<EOF
.x trd_hits.C($NEVENTS)
.q
EOF

root -b <<EOF
.x trd_reco.C($NEVENTS)
.q
EOF

