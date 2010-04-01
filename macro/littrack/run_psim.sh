#!/bin/sh

#. ./psim.sh 1

xterm -hold -e ". ./psim.sh 1"&
xterm -hold -e ". ./psim.sh 2"&
xterm -hold -e ". ./psim.sh 3"&
xterm -hold -e ". ./psim.sh 4"&
xterm -hold -e ". ./psim.sh 5"&

#bsub -q betch64 -J mc.$XXXX.run -o log/$XXXX.log -N -u andrey.lebedev@gsi.de sh "psim.sh 1"