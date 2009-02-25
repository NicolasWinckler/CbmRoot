root -l <<EOF
.x run_sim.C
.q
EOF
root -l << EOF
.x run_reco.C
.q
EOF