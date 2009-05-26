for p1 in 12 15 18 21
do
	for p2 in 2 3 5
	do
        root -l -q run_reco_param_opt_ht.C\($p1,$p2\)

    done

done
 
