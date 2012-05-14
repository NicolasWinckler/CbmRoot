#!/bin/sh

xterm -hold -e ". ./trd_qa.sh trd_v10b_smearing smearing trd/trd_v10b.geo $VMCWORKDIR/parameters/trd/trd_v10b.digi.par"&
xterm -hold -e ". ./trd_qa.sh trd_v11c_smearing smearing trd/trd_v11c.geo $VMCWORKDIR/parameters/trd/trd_v11c.digi.par"&
xterm -hold -e ". ./trd_qa.sh trd_v11d_smearing smearing trd/trd_v11d.geo $VMCWORKDIR/parameters/trd/trd_v11d.digi.par"&
