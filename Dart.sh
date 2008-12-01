#!/bin/bash

##################################################################
# set the source and the build directory for ctest
# change this directories to your needs
# set the SIMPATH to your needs
# LINUX_FLAVOUR should be set to the distribution you are using
# eg Debian, SuSe etc.
# For example
#export LINUX_FLAVOUR=Etch32
#export FAIRSOFT_VERSION=mar08
#export SIMPATH=/misc/cbmsoft/${LINUX_FLAVOUR}/${FAIRSOFT_VERSION}/fairsoft
#export BUILDDIR=/tmp/fairroot/build_cbm_${FAIRSOFT_VERSION}
#export SOURCEDIR=/misc/uhlig/SVN/ctest/cbmroot
##################################################################

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

export LABEL1=${LINUX_FLAVOUR}-$SYSTEM-$COMPILER$GCC_VERSION-fairsoft_$FAIRSOFT_VERSION
export LABEL=$(echo $LABEL1 | sed -e 's#/#_#g')
export SITE=$(hostname -f)

# get the number of processors
export number_of_processors=$(cat /proc/cpuinfo | grep processor | wc -l)

echo "************************"
date
echo "LABEL: " $LABEL
echo "SITE: " $SITE
echo "Model: " ${ctest_model}
echo "Nr. of processes: " $number_of_processors
echo "************************"

cd $SOURCEDIR
ctest -S $SOURCEDIR/CbmRoot_test.cmake -V --VV 