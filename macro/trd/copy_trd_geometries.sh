#
##   install additional trd geometries from macro/trd
#

echo \# copy geometries
cp -v geometry/trd.v11/*.geo      ../../geometry/trd
echo
echo \# copy digi.par files
cp -v geometry/trd.v11/*.digi.par ../../parameters/trd
