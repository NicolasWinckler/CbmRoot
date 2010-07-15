#!/bin/bash
# create for a list of geiometries the corresponding trd digitization
# parameters

directory=jul10

for file in $directory/*.geo; do
 echo "Creating trd parameters for $file"
 root -l create_geometry_file.C\(\"$file\"\)
 root -l create_digipar.C\(\"$file\"\)
 fileName=$(basename $file)
 fileNameNoExt=${fileName%.*}
 mv $fileNameNoExt.digi.par $directory
 mv geofile_$fileNameNoExt.root $directory
done

echo ""
echo ""
echo "Please remember to delete the last line in all the created"
echo "parameter files."
