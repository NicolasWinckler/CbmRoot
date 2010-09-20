#for param in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
for iter in 1 2 3 4 5 
do
for param in 8 16 24 32 40 48 56 64 72 80
do
root -l<<EOF
.x run_parallel_qa.C(100, $param)
.q
EOF
sleep 2
done
done