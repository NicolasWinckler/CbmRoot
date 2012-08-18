#!/bin/bash
# create for a list of geometries the corresponding trd digitization
# parameters

#directory=jul10
#directory=../macro/trd/geometry/trd.v11
#directory=geometry/trd.v11
directory=geometry

for file in $directory/*.geo; do
 echo "Creating trd parameters for $file"
 root -l -b -q create_geometry_file.C\(\"$file\"\)
 root -l -b -q create_digipar.C\(\"$file\"\)
#[ -x digipar_cut_lf ] echo digipar_cut_lf digipar_cut_lf.cut
# # fileName=$(basename $file)
# # fileNameNoExt=${fileName%.*}
# # echo mv $fileNameNoExt.digi.par $directory
# # echo mv geofile_$fileNameNoExt.root $directory
done

echo ""
echo ">> Please remember to delete the last line in all the created parameter files. <<"
echo "cut_digipar_lf file.digi.par.long file.digi.par"
echo ""
