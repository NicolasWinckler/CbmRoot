# Copyright (C) 2005 Florian Uhlig <f.uhlig@gsi.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_INIT([cbmroot],[1.0],[f.uhlig@gsi.de],cbmroot)
AC_CONFIG_AUX_DIR(config)
#AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])

AC_PROG_CC
AC_PROG_CXX
#AC_PROG_RANLIB
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

#AC_PATH_PROG(ROOT_FOUND, root-config , no)
#if test "$ROOT_FOUND" = "no"; then
#  echo ""
#  echo "You're missing ROOT. If your at GSI, please run the following " 
#  echo "command and rerun configure again. "
#  echo ". rootlogin new "
#  echo "If you're not at GSI, but you're that ROOT is installed "
#  echo "on your machine, include the path to root-config in your PATH."
#  echo ""
#  AC_MSG_ERROR(No ROOT instalation found)
#fi


#LD_LIBRARY_PATH=""
ROOT_PATH(4.00/08, , AC_MSG_ERROR(Your ROOT version is too old))
SIM_PATH(,AC_MSG_ERROR(Couldn't find your instalation of simulation packages))

AC_ARG_ENABLE(optimize, [  --enable-optimize     allow to optimize code 
               [defalut=no]], enable_optimize=$enableval, enable_optimize=no)

#  the flag "-ffriend-injection" is needed for pluto and gcc4.1.0 M.Al-Turany

GCC_MAJOR="$(gcc -dumpversion 2>&1 | cut -d'.' -f1)"
GCC_MINOR="$(gcc -dumpversion 2>&1 | cut -d'.' -f2)"

FRIENDFLAG=""
if test "$GCC_MAJOR" = "4"; then
	if test "$GCC_MINOR" != "0"; then
		FRIENDFLAG="-ffriend-injection"
	fi
fi


if test "$enable_optimize" = "yes"; then
   CPPFLAGS="$CPPFLAGS -O2 -pipe -Wall  -Wno-deprecated $FRIENDFLAG"
   CXXFLAGS=""
else
   CPPFLAGS="$CPPFLAGS -g -pipe -Wall   -Wno-deprecated $FRIENDFLAG "
   CXXFLAGS=""
fi

AC_ARG_ENABLE(geant4, [  --enable-geant4     allow to switch on/off compilation of geant 4 
               [defalut=no]], geant4=$enableval, geant4=no)
AM_CONDITIONAL(COMP_GEANT4, test "$geant4" = "yes")

AC_ARG_ENABLE(geant3, [  --enable-geant3     allow to switch on/off compilation of geant 3
                [defalut=no]], geant3=$enableval, geant3=no)
AM_CONDITIONAL(COMP_GEANT3, test "$geant3" = "yes")

AC_ARG_ENABLE(oracle, [  --enable-oracle     allow to switch on/off compilation of oracle
                [defalut=no]], oracle=$enableval, oracle=no)
if test "$oracle" = "yes" -a "$ROOTSIMSYSTEM" = "gsi"; then  
   AC_PATH_PROG(ORACLE_FOUND, osh, no)
   if test "$ORACLE_FOUND" = "no"; then
     AM_CONDITIONAL(COMP_ORACLE, test "$oracle" = "no")
   else 
     AM_CONDITIONAL(COMP_ORACLE, test "$oracle" = "yes")
     ORA_USER="CBM_ANA_PUBLIC/CBM@DB-CBM"
     AC_SUBST(ORA_USER)
#     LD_LIBRARY_PATH="$ORACLE_HOME/lib:$LD_LIBRARY_PATH"
   fi
else 
  AM_CONDITIONAL(COMP_ORACLE,  test "$oracle" = "yes" -a "$ROOTSIMSYSTEM" = "gsi")
fi


AC_SUBST(LD_LIBRARY_PATH)

AC_CONFIG_FILES([
   Makefile
   base/Makefile
   geobase/Makefile
   parbase/Makefile
   cbmbase/Makefile
   field/Makefile
   generators/Makefile
   passive/Makefile
   rich/Makefile
   mvd/Makefile
   sts/Makefile
   tof/Makefile
   trd/Makefile
   cbmg4/Makefile
   ecal/Makefile
   zdc/Makefile
   much/Makefile
   lhetrack/Makefile   
   littrack/Makefile   
   global/Makefile
   KF/Makefile
   L1/Makefile
   htrack/Makefile
   dilept/Makefile
])
#   ora/Makefile

AC_OUTPUT

VMCWORKDIR=`cd "$srcdir" && pwd`
AC_SUBST(VMCWORKDIR)

CBMLIBDIR=`cd "$ac_abs_top_builddir" && pwd`
AC_SUBST(CBMLIBDIR)

if test "$ORACLE_FOUND" = "no"; then
  echo ""
  echo "You're missing Oracle. Everything will be compiled without "
  echo "Oracle support. If your at GSI, please run the following command "
  echo "and rerun configure again. "
  echo ""
  echo ". oraenv "
  echo ""
  echo "If you're not at GSI, but you're shure that Oracle is installed "
  echo "on your machine, please set the required environment variables"
  echo ""
fi

LD_LIBRARY_PATH="$CBMLIBDIR/lib:$LD_LIBRARY_PATH"

echo "#----------------------------------" > config.sh
echo "# Setting the environment variables" >> config.sh
echo "#----------------------------------" >> config.sh
echo "" >> config.sh
echo "export ROOTSIM='$ROOTSIM'" >> config.sh
echo "export GEANT4LIBDIR='$GEANT4LIBDIR'" >> config.sh 
echo "export GEANT4INC='$GEANT4INC'" >> config.sh
echo "export PLUTOLIBDIR='$PLUTOLIBDIR'" >> config.sh
echo "export PLUTOINC='$PLUTOINC'" >> config.sh
echo "export GEANT4VMCLIBDIR='$GEANT4VMCLIBDIR'" >> config.sh
echo "export GEANT4VMCINC='$GEANT4VMCINC'" >> config.sh 
echo "export CLHEPLIBDIR='$CLHEPLIBDIR'" >> config.sh
echo "export CLHEPINC='$CLHEPINC'" >> config.sh
echo "export GEANT3LIBDIR='$GEANT3LIBDIR'" >> config.sh
echo "export GEANT3INC='$GEANT3INC'" >> config.sh
echo "export ROOTLIBDIR='$ROOTLIBDIR'" >> config.sh 
echo "export ROOTINCDIR='$ROOTINCDIR'" >> config.sh
echo "export ROOTCFLAGS='$ROOTCFLAGS'" >> config.sh
echo "export ROOTLIBS='$ROOTLIBS'" >> config.sh
echo "export ROOTGLIBS='$ROOTGLIBS'" >> config.sh
echo "export ROOTAUXLIBS='$ROOTAUXLIBS'" >> config.sh
echo "export ROOTAUXCFLAGS='$ROOTAUXCFLAGS'" >> config.sh
echo "export ROOTRPATH='$ROOTRPATH'" >> config.sh
echo "export ROOTSYS='$ROOTSYS'" >> config.sh
echo "export VMCWORKDIR='$VMCWORKDIR'" >> config.sh
echo "export VGM_INSTALL='$VGM_INSTALL'" >> config.sh
echo "export CBMLIBDIR='$CBMLIBDIR'" >> config.sh
echo "export LD_LIBRARY_PATH='$LD_LIBRARY_PATH'" >> config.sh
echo "export PATH=$ROOTSYS/bin:$PATH" >> config.sh
echo "" >> config.sh
echo "#----------------------------------" >> config.sh
echo "# Setting up the Oracle environment" >> config.sh
echo "#----------------------------------" >> config.sh
echo "" >> config.sh 
if test !"$ORACLE_FOUND" = "no"; then
 echo ""  >> config.sh
 echo ". oraenv"  >> config.sh
 echo "export ORA_USER='$ORA_USER'" >> config.sh
fi
echo "#----------------------------------" > config.csh
echo "# Setting the environment variables" >> config.csh
echo "#----------------------------------" >> config.csh
echo "" >> config.csh
echo "setenv ROOTSIM '$ROOTSIM'" >> config.csh
echo "setenv GEANT4LIBDIR '$GEANT4LIBDIR'" >> config.csh 
echo "setenv GEANT4INC '$GEANT4INC'" >> config.csh
echo "setenv PLUTOLIBDIR '$PLUTOLIBDIR'" >> config.csh
echo "setenv PLUTOINC '$PLUTOINC'" >> config.csh
echo "setenv GEANT4VMCLIBDIR '$GEANT4VMCLIBDIR'" >> config.csh
echo "setenv GEANT4VMCINC '$GEANT4VMCINC'" >> config.csh 
echo "setenv CLHEPLIBDIR '$CLHEPLIBDIR'" >> config.csh
echo "setenv CLHEPINC '$CLHEPINC'" >> config.csh
echo "setenv GEANT3LIBDIR '$GEANT3LIBDIR'" >> config.csh
echo "setenv GEANT3INC '$GEANT3INC'" >> config.csh
echo "setenv ROOTLIBDIR '$ROOTLIBDIR'" >> config.csh 
echo "setenv ROOTINCDIR '$ROOTINCDIR'" >> config.csh
echo "setenv ROOTCFLAGS '$ROOTCFLAGS'" >> config.csh
echo "setenv ROOTLIBS '$ROOTLIBS'" >> config.csh
echo "setenv ROOTGLIBS '$ROOTGLIBS'" >> config.csh
echo "setenv ROOTAUXLIBS '$ROOTAUXLIBS'" >> config.csh
echo "setenv ROOTAUXCFLAGS '$ROOTAUXCFLAGS'" >> config.csh
echo "setenv ROOTRPATH '$ROOTRPATH'" >> config.csh
echo "setenv ROOTSYS '$ROOTSYS'" >> config.csh
echo "setenv VMCWORKDIR '$VMCWORKDIR'" >> config.csh
echo "setenv CBMLIBDIR '$CBMLIBDIR'" >> config.csh
echo "setenv LD_LIBRARY_PATH '$LD_LIBRARY_PATH'" >> config.csh
echo "setenv PATH '$ROOTSYS/bin:$PATH'" >> config.csh
echo "" >> config.csh
echo "#----------------------------------" >> config.csh
echo "# Setting up the Oracle environment" >> config.csh
echo "#----------------------------------" >> config.csh
echo "" >> config.csh 
