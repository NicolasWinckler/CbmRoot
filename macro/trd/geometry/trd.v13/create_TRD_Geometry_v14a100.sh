#
# create TRD geometry from patch file
#
SOURCE=Create_TRD_Geometry_v14a300.C
TARGET=Create_TRD_Geometry_v14a100.C
PATCH=a3toa1.patch
#
echo
echo cp $SOURCE $TARGET
echo patch $TARGET $PATCH
echo root -l $TARGET
echo
#
cp $SOURCE $TARGET
patch $TARGET $PATCH
root -l $TARGET


