#!/bin/bash

for X in 10 11 12 13 14 15 16 17 18 19 20; do
   root -b -q "./thermal_JPsi_25GeV.C($X)"
done
 