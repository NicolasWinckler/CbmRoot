#!/bin/bash

##################################################################
# set the source and the build directory for ctest
# change this directories to your needs
# set the SIMPATH to your needs
# LINUX_FLAVOUR should be set to the distribution you are using
# eg Debian, SuSe etc.
# For example
# export SIMPATH=/misc/cbmsoft/Debian3.1/new
# export BUILDDIR=/misc/uhlig/test/mytest2/build_cbmroot
# export SOURCEDIR=/misc/uhlig/test/mytest2/cbmroot
# export LINUX_FLAVOUR=Debian
##################################################################
export SIMPATH=
export BUILDDIR=
export SOURCEDIR=
export LINUX_FLAVOUR=
###################################################################

if test  "x$SIMPATH" = "x" ; then
  echo ""                                                             
  echo "-- Error -- You don't set the needed variables in this shell script."
  echo "-- Error -- Please edit the script and do so."
  echo ""
  exit 1
fi

if [ "$#" -lt "1" ]; then
  echo ""
  echo "-- Error -- Please start script with the ctest model as an argument."
  echo "-- Error -- Possible arguments are Nightly and Experimental ."
  echo ""
  exit 1
fi

# set the ctest model to command line parameter
export ctest_model=$1

# extract information about the system and the machine and set
# environment variables used by ctest
SYSTEM=$(uname -o)-$(uname -m)
if test -z $CXX ; then
  COMPILER=gcc;
  GCC_VERSION=$(gcc -dumpversion)
else
  COMPILER=$CXX;
  GCC_VERSION=$($CXX -dumpversion)
fi
export LABEL=${LINUX_FLAVOUR}-$SYSTEM-$COMPILER$GCC_VERSION
export SITE=$(hostname -f)

echo "************************"
date
echo "LABEL: " $LABEL
echo "SITE: " $SITE
echo "Model: " ${ctest_model}
echo "************************"
 


# run ctest to clear the build directory, update the project from svn 
# and to create the config script for the project, submit results to
# the server
ctest -S $SOURCEDIR/CbmRoot_test1.cmake,${ctest_model} -V --VV --debug

# runing the config script sets all needed environment variables
. $BUILDDIR/config.sh

# create directory where run macros store their output
cd $BUILDDIR
mkdir macro/run/data

# update from svn, build makefiles, build libraries, run tests and submit
# results to the server
cd $SOURCEDIR
ctest -S $SOURCEDIR/CbmRoot_test.cmake,${ctest_model}  -V --VV --debug
