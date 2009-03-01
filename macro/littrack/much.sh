#!/bin/sh
cd $MYBUILDDIR
. ./config.sh
cd -
root -b <<EOF
.x much_sim.C($NEVENTS)
.q
EOF

root -b <<EOF
.x sts_reco.C($NEVENTS)
.q
EOF

root -b <<EOF
.x much_hits.C($NEVENTS)
.q
EOF

root -b <<EOF
.x much_reco.C($NEVENTS)
.q
EOF
