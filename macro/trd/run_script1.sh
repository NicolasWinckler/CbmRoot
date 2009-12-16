. /u/slebedev/JUL09/buildSC/config.sh
#root -l<<EOF
#.x /u/slebedev/JUL09/trunk/macro/trd/trd_elid_sim_many.C($1, 1)
#.q
#EOF
#for param in 1 2 3
#do
#root -l<<EOF
#.x /u/slebedev/JUL09/trunk/macro/trd/trd_elid_reco_many.C($1, 1,$param)
#.q
#EOF
#done
for sigma in 0.01e-6 0.025e-6 0.05e-6 0.1e-6 0.25e-6 0.5e-6 
do
for param in 1 2 3
do
root -l<<EOF
.x /u/slebedev/JUL09/trunk/macro/trd/trd_elid_trainANN_txt.C($param, $1, $sigma)
.q
EOF
done
done