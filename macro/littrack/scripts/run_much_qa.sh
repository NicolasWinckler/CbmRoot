#!/bin/sh

xterm -hold -e ". ./much_qa.sh much_v11a much/much_v11a.geo $VMCWORKDIR/parameters/much/much_v11a.digi.root yes"&
xterm -hold -e ". ./much_qa.sh much_v12a much/much_v12a.geo $VMCWORKDIR/parameters/much/much_v12a.digi.root yes"&
xterm -hold -e ". ./much_qa.sh much_v11a_signal much/much_v11a.geo $VMCWORKDIR/parameters/much/much_v11a.digi.root no"&
xterm -hold -e ". ./much_qa.sh much_v12a_signal much/much_v12a.geo $VMCWORKDIR/parameters/much/much_v12a.digi.root no"&