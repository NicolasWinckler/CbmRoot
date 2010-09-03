#!/bin/sh

# Track reconstruction tests
xterm -hold -e ". ./sim.sh muon 1000 branch"&
xterm -hold -e ". ./sim.sh electron 500 branch"&
xterm -hold -e ". ./sim.sh muon 1000 nn"&
xterm -hold -e ". ./sim.sh electron 500 nn"&
xterm -hold -e ". ./sim.sh muon 1000 nn_parallel"&
xterm -hold -e ". ./sim.sh electron 500 nn_scalar"&

# Track fit tests
xterm -hold -e ". ./fit.sh muon 5000 std"&
xterm -hold -e ". ./fit.sh electron 5000 std"&
xterm -hold -e ". ./fit.sh muon 5000 fast"&
xterm -hold -e ". ./fit.sh electron 5000 fast"&
