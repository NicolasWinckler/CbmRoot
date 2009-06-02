for number in 5 6 7 8 9
do
root -l<<EOF
.x trd_elid_sim_many.C($number, 1)
.q
EOF
done
 
