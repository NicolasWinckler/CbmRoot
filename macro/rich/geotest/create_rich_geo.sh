#!/bin/sh

for Z in 0 1 2 3 4 5; do
   export DIR=/d/cbm06/user/slebedev/geotest
   export XXXX=geodat/rich_compact000$Z
   root -b -q "../create_RICH_geo_file.C(\"$XXXX\")"
done;

