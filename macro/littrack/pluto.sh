#!/bin/bash

for X in 0 1 2 3 4 5 6 7 8 9; do
   root -b -q "./pluto_jpsi_25gev.C($X)"
done
 