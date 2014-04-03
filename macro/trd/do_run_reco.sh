#!/bin/bash
nThreads=0
maxThreads=0
nEvents=500
cat /proc/cpuinfo | grep processor | tail -n1 | cut -c 13- > temp
while read line 
do
    maxThreads=$line
    echo "${maxThreads} threads are started in parallel"
done <temp
rm temp  
 maxThreads=3
for urqmd in {0..199}
do
	nThreads=$(($nThreads+1))
	echo "${nThreads} root -l -b -q run_sim.C'('$nEvents,$urqmd')'"
	echo "nice root -l -b -q run_reco.C'('$nEvents,$urqmd')' &"  > myreco
	. myreco   &> /dev/null
#&>> logrecofile.txt
	sleep 10
	if [ "$nThreads" -ge "$maxThreads" ]; then
	    wait ${!}
	    nThreads=0
	fi
done
