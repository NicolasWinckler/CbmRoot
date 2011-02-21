#!/bin/sh

# Track reconstruction tests
xterm -hold -e ". ./sim.sh muon 100 branch"&
xterm -hold -e ". ./sim.sh electron 100 branch"&
xterm -hold -e ". ./sim.sh muon 100 nn"&
xterm -hold -e ". ./sim.sh electron 100 nn"&
xterm -hold -e ". ./sim.sh muon 100 nn_parallel"&
xterm -hold -e ". ./sim.sh electron 100 nn_scalar"&

# Track fit tests
xterm -hold -e ". ./fit.sh muon 100 std"&
xterm -hold -e ". ./fit.sh electron 100 std"&
xterm -hold -e ". ./fit.sh muon 100 fast"&
xterm -hold -e ". ./fit.sh electron 100 fast"&
