for p1 in -0.5 -0.55 -0.6 -0.65 -0.7
do
	for p2 in 2.0 2.25 2.5 3
	do
		for p3 in 0.8 1 1.2 1.5
		do
			for p4 in 0.3 0.35 0.4
			do
				for p5 in 0.3 0.35 0.4
				do
        			root -l -q run_reco_param_opt_ht.C\($p1,$p2,$p3,$p4,$p5\)
				done
			done
		done
    done
done
 
