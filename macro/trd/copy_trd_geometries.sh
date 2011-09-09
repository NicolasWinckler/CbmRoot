#
##   install additional trd geometries from macro/trd
#

echo \# copy geometries
cp -v geometry/trd.v11/v11?/*.geo* ../../geometry/trd
#cp -v geometry/trd.v11/*.geo       ../../geometry/trd
echo
echo \# copy digi.par files
cp -v geometry/trd.v11/v11?/*.digi.par ../../parameters/trd
cp -v geometry/trd.v11/v11b/trd_v11b_7.5mm.digi.par ../../parameters/trd/trd_v11b.digi.par
cp -v geometry/trd.v11/v11a/trd_v11a_7.5mm.digi.par ../../parameters/trd/trd_v11a.digi.par
#cp -v geometry/trd.v11/*.digi.par ../../parameters/trd
