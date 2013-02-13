#!/bin/bash
# create for a list of geometries in root format
# the corresponding trd digitization parameters

#directory=geometry/trd.v13
directory=geometry

for file in $directory/*.root; do
 echo "Creating trd parameters for $file"
 root -l -b -q create_geometry_file_root.C\(\"$file\"\)
 root -l -b -q create_digipar_root.C\(\"$file\"\)
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
