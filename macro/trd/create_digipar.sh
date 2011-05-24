#!/bin/bash
# create for a list of geiometries the corresponding trd digitization
# parameters

#directory=jul10
#directory=../macro/trd/geometry/trd.v11
directory=geometry/trd.v11

for file in $directory/*.geo; do
 echo "Creating trd parameters for $file"
 root -l create_geometry_file.C\(\"$file\"\)
 root -l create_digipar.C\(\"$file\"\)
# fileName=$(basename $file)
# fileNameNoExt=${fileName%.*}
# echo mv $fileNameNoExt.digi.par $directory
# echo mv geofile_$fileNameNoExt.root $directory
done

echo ""
echo ""
echo "Please remember to delete the last line in all the created"
echo "parameter files."
