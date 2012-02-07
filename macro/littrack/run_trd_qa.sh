#!/bin/sh

# Parameters to bash script trd_qa.sh: 
# 1) number of events; 
# 2) test name; 
# 3) TRD hit producer type; 
# 4) TRD geometry file name; 
# 5) TRD digi parameter file name

# Current standard geometry
xterm -hold -e ". ./trd_qa.sh 500 trd_v10b_smearing smearing trd/trd_v10b.geo"&

# Old Geometries for comparison
xterm -hold -e ". ./trd_qa.sh 500 trd_v11a_smearing smearing trd/trd_v11a.geo"&
#xterm -hold -e ". ./trd_qa.sh 2 trd_v11b_smearing smearing trd/trd_v11b.geo"&
#xterm -hold -e ". ./trd_qa.sh 2 trd_v10c_smearing smearing trd/trd_v10c.geo"&

#xterm -hold -e ". ./trd_qa.sh 2 trd_v11b_clustering clustering trd/trd_v11b.geo $VMCWORKDIR/parameters/trd/trd_v11b.digi.par"&
#xterm -hold -e ". ./trd_qa.sh 2 trd_v10c_clustering clustering trd/trd_v10c.geo $VMCWORKDIR/parameters/trd/trd_v10c.digi.par"&

# New geometries
xterm -hold -e ". ./trd_qa.sh 500 trd_v11c_smearing smearing trd/trd_v11c.geo"&
#xterm -hold -e ". ./trd_qa.sh 2 trd_v11d_smearing smearing trd/trd_v11d.geo"&
#xterm -hold -e ". ./trd_qa.sh 2 trd_v12a_smearing smearing trd/trd_v12a.geo"&

# Clustering
xterm -hold -e ". ./trd_qa.sh 100 trd_v10b_clustering clustering trd/trd_v10b.geo $VMCWORKDIR/parameters/trd/trd_v10b.digi.par"&
##xterm -hold -e ". ./trd_qa.sh 2 trd_v11a_clustering clustering trd/trd_v11a.geo $VMCWORKDIR/parameters/trd/trd_v11a.digi.par"&
xterm -hold -e ". ./trd_qa.sh 74 trd_v11c_clustering clustering trd/trd_v11c.geo $VMCWORKDIR/parameters/trd/trd_v11c.digi.par"&
